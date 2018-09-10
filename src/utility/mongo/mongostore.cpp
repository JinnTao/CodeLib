﻿

#include <ratio>
#include <iostream>
#include "mongostore.h"
#include "logger.h"
#include "bsoncxx/types.hpp"
#include "bsoncxx/stdx/string_view.hpp"
mongocxx::instance MongoStore::instance_ = {};

MongoStore::~MongoStore() {
    is_running_.store(false, std::memory_order_release);
}

int32 MongoStore::init(const mongoConfig& mongo_config) {
    ILOG("MongoDb Init uri:{},database:{}", mongo_config.address, mongo_config.database);
    try {
        config_        = mongo_config;
        string address = string(mongo_config.address);
        uri_           = {address};
        client_        = {uri_};
        db_            = client_.database(string(mongo_config.database));

    } catch (const std::exception& e) {
        WLOG("MongoDb init failed!Msg:{}.", e.what());
        return -1;
    }

    return 0;
}

int32 MongoStore::start() {
    is_running_.store(true, std::memory_order_release);

    return 0;
}

int32 MongoStore::stop() {
    is_running_.store(false, std::memory_order_release);
    return 0;
}
bool MongoStore::getData(string                                             collectionName,
                         std::chrono::time_point<std::chrono::system_clock> sTimePoint,
                         std::chrono::time_point<std::chrono::system_clock> eTimePoint,
                         std::vector<double>&                               close,
                         std::vector<double>&                               open,
                         std::vector<double>&                               high,
                         std::vector<double>&                               low,
                         std::vector<int32_t>&                              volume,
                         std::vector<string>&                               dateTime) {
    try {
        using bsoncxx::builder::stream::close_array;
        using bsoncxx::builder::stream::close_document;
        using bsoncxx::builder::stream::document;
        using bsoncxx::builder::stream::finalize;
        using bsoncxx::builder::stream::open_array;
        using bsoncxx::builder::stream::open_document;

        using bsoncxx::builder::basic::kvp;

        mongocxx::collection               coll = db_.collection(collectionName);
        bsoncxx::builder::stream::document filter_builder;

        std::chrono::duration<int, std::ratio<60 * 60>> one_hour(1);
        sTimePoint            = sTimePoint + 8 * one_hour;
        eTimePoint            = eTimePoint + 8 * one_hour;
        std::time_t endTime   = std::chrono::system_clock::to_time_t(eTimePoint);
        std::time_t startTime = std::chrono::system_clock::to_time_t(sTimePoint);

        // std::cout << " s " << std::ctime(&startTime) << " e " << std::ctime(&endTime) << std::endl;
        filter_builder << "recordTime" << open_document << "$gte" << bsoncxx::types::b_date(sTimePoint) << "$lte"
                       << bsoncxx::types::b_date(eTimePoint) << close_document;
        bsoncxx::builder::stream::document sort_filter;
        sort_filter << "recordTime" << 1;
        mongocxx::options::find out;
        out.sort(sort_filter.view());

        mongocxx::cursor cursor = coll.find(filter_builder.view(), out);

        for (auto&& doc : cursor) {
            close.push_back(doc["close"].get_double());
            high.push_back(doc["high"].get_double());
            low.push_back(doc["low"].get_double());
            open.push_back(doc["open"].get_double());
            volume.push_back(doc["marketVol"].get_int32());
            std::chrono::system_clock::time_point DateTime = doc["recordTime"].get_date();
            std::time_t                           c     = std::chrono::system_clock::to_time_t(DateTime - 8 * one_hour);
            string                                timeS = std::ctime(&c);
            timeS.pop_back();
            dateTime.push_back(timeS);

            ILOG("collectionName:{},{},close:{},high:{},low:{},open:{}",
                 collectionName,
                 dateTime.at(dateTime.size() - 1),
                 doc["close"].get_double(),
                 doc["high"].get_double(),
                 doc["low"].get_double(),
                 doc["open"].get_double());
        }
        // LOG(INFO) << " finish import Data";
        return true;

    } catch (const std::exception e) {
        // LOG(INFO) << "Mongo Query faild! " << e.what();
        return false; 
    }
}

bool MongoStore::getData(string collection_name, int data_length, std::vector<barData>& bar_data_vec) {
    try {
        using bsoncxx::builder::stream::close_array;
        using bsoncxx::builder::stream::close_document;
        using bsoncxx::builder::stream::document;
        using bsoncxx::builder::stream::finalize;
        using bsoncxx::builder::stream::open_array;
        using bsoncxx::builder::stream::open_document;

        using bsoncxx::builder::basic::kvp;

        mongocxx::collection               coll = db_.collection(collection_name);
        bsoncxx::builder::stream::document sort_filter;
        sort_filter << "recordTime" << -1;
        mongocxx::options::find out;
        out.sort(sort_filter.view());
        out.limit(data_length);
        mongocxx::cursor                                cursor = coll.find(sort_filter.view(), out);
        std::chrono::duration<int, std::ratio<60 * 60>> one_hour(1);
        for (auto&& doc : cursor) {

            std::chrono::system_clock::time_point DateTime = doc["recordTime"].get_date();
            std::time_t                           c     = std::chrono::system_clock::to_time_t(DateTime - 8 * one_hour);
            string                                timeS = std::ctime(&c);
            timeS.pop_back();
            barData bar_data_;
            bar_data_.close          = doc["close"].get_double();
            bar_data_.open           = doc["open"].get_double();
            bar_data_.high           = doc["high"].get_double();
            bar_data_.low            = doc["low"].get_double();
            bar_data_.date_time      = DateTime - 8 * one_hour;// convert to utf8
            bar_data_.volume         = doc["marketVol"].get_int32();
            bsoncxx::stdx::string_view view = doc["exchange"].get_utf8().value;
            bar_data_.exchange              = view.to_string();
            bar_data_.openInterest          = doc["OpenInterest"].get_double();
            bar_data_.collection_symbol     = collection_name;
            bar_data_.symbol                = doc["id"].get_utf8().value.to_string();
            bar_data_vec.push_back(bar_data_);
        }
        auto start_date = bar_data_vec.end()->date_time;
        auto start_date_tm  = std::chrono::system_clock::to_time_t(start_date);
        string start_date_str = std::ctime(&start_date_tm);

        auto   end_date     = bar_data_vec.begin()->date_time;
        auto   end_date_tm  = std::chrono::system_clock::to_time_t(end_date);
        string end_date_str = std::ctime(&end_date_tm);
        ILOG("Load {} history data Datetime from {} to {}",collection_name,start_date_str,end_date_str);
        return true;
    } catch (const std::exception e) {
        ILOG("Mongo Query failed:{}", e.what());
        return false;
    }
}