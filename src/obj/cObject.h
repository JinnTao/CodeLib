#pragma once
#include <string>
#include <chrono>
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
    double                                             volume;
    double                                             openInterest;
};

class ArrayManager {
    //K�����й����ߣ�����
    //1. K��ʱ�����е�ά��
    //2. ���ü���ָ��ļ���


};