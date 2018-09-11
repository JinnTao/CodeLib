#pragma once
#include <mutex>
#include <atomic>
#include <csignal>

#include "ThostFtdcUserApiDataType.h"



struct ctpConfig {
    TThostFtdcBrokerIDType   brokerId;
    TThostFtdcInvestorIDType userId;
    char                     passwd[252];

    char mdAddress[50];
    char tdAddress[50];
    char md_flow_path_[50];
    char td_flow_path_[50];
    ctpConfig() { memset(this, 0, sizeof(ctpConfig)); }
    void reset() { memset(this, 0, sizeof(ctpConfig)); }
};

struct mongoConfig {
    char address[50];  // Host address
    char database[50];
    int  mongoPort;     //
    bool mongoLogging;  //

    mongoConfig() { memset(this, 0, sizeof(mongoConfig)); }
    void reset() { memset(this, 0, sizeof(mongoConfig)); }
};

struct strategyConfig {
    char tradeDayDir[125];
    char dataBaseDir[125];

    char startDate[25];
    char endDate[25];

    // dataSource
    char startDateTime[150];
    char endDateTime[150];
    char collectionName[20];

    // kingKeltNer
    //;����ͨ����ֵ�Ĵ�����
    double kkLength;  // = 11
    //;����ͨ����ȵ�ƫ��
    double kkDev;  // = 1.6
    //;�ƶ�ֹ��
    double trailingPrcnt;  // = 0.8
    //;ÿ�ν��׵�����
    double fixedSize;  // = 1
    //;��ʼ���������õ�����
    double initDays;  // = 10

    // strategy base Information
    char inst[300];            // strategy Trading Inst
    char lots[300];            // strategy Trading Inst lost
    char timeMode[300];        // strategy Time filter mode
    char collectionList[500];  // strategy collection name

    strategyConfig() { memset(this, 0, sizeof(strategyConfig)); }
    void reset() { memset(this, 0, sizeof(strategyConfig)); }
};

struct sInstrumentInfo {
    char   InstrumentID[32];
    char   ExchangeID[32];
    char   ProductID[32];
    int    MaxMarketOrderVolume;
    int    MinMarketOrderVolume;
    int    MaxLimitOrderVolume;
    int    MinLimitOrderVolume;
    int    VolumeMultiple;
    double PriceTick;
    int    IsTrading;
    double LongMarginRatio;
    double ShortMarginRatio;
};

struct sTradingAccountInfo {
    char   BrokerID[32];
    char   AccountID[32];
    double PreBalance;
    double Balance;
    double Available;
    double WithdrawQuota;
    double CurrMargin;
    double CloseProfit;
    double PositionProfit;
    double Commission;
    double FrozenMargin;
};

// Direction Offset
namespace traderTag {
enum DIRECTION { buy, sell };
enum OFFSETFLAG { open, close };
}  // namespace traderTag

namespace global {
extern std::mutex                 init_mutex;
extern std::mutex                 run_mutex;
extern std::atomic<bool>          need_reconnect;
extern volatile std::sig_atomic_t is_running;
extern sTradingAccountInfo        account_info;
}  // namespace global
