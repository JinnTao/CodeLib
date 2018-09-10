#pragma once
#include <string>
#include <chrono>
#include <vector>
#include "common.h"
#include "ta-lib/include/ta_func.h"

class barData {
public:
    barData();
    ~barData();
    std::string collection_symbol;
    std::string symbol;
    std::string exchange;

    double open;
    double high;
    double low;
    double close;

    std::chrono::time_point<std::chrono::system_clock> date_time;  // bar�Ŀ�ʼʱ�� ����
    int                                             volume;
    int                                             openInterest;
};

class ArrayManager {
    // K�����й����ߣ�����
    // 1. K��ʱ�����е�ά��
    // 2. ���ü���ָ��ļ���
private:
    // ʱ��Ӿɵ��ϣ�����begin 2018,��end��ʾ2019
    std::vector<double> high_;   // ��߼�����
    std::vector<double> low_;    // ��ͼ�����
    std::vector<double> close_;  // ���̼�����
    std::vector<double> open_;
    std::vector<int32>  vol_;
    std::vector<int32>  open_interest_;//�ֲ�������
    std::vector<std::chrono::system_clock::time_point> date_time_; 
    int                                                count_;  //�������
    int                                                size_;   //�����С
    bool                                               inited_;
    bool                                               is_tradable_;

public:
    ArrayManager(int size = 100);
    void                                               update(barData bar);
    void                                               fresh(barData bar);
    std::vector<double>                                high();
    std::vector<double>                                low();
    std::vector<double>                                close();
    std::vector<double>                                open();
    std::vector<int32>                                 vol();
    std::vector<int32>                                 open_interest();
    std::vector<std::chrono::system_clock::time_point> date_time();
    bool                                               is_tradable();
    barData                                            lastBarData();
    void                                               setTradable(bool);
    // ����ָ��
    bool keltner(int n, double dev, double& up, double& down);


};