#pragma once
#include "cstrategy.h"

class cStrategyKingKeltner :
    public cStrategy
{
protected:
private:
    string collection_name_;

    bool m_oldState;

    std::map<std::string, int32> m_instLotsList;
    // init data
    string m_startDate;
    string m_endDate;

    //策略输入参数
    double m_kkLength;
    double m_kkDev;

    // 策略参数变量
    double m_kkUp;
    double m_kkDown;
    double m_intraTradeHigh;
    double m_intraTradeLow;

    int m_netPos;

    double m_orderBuyPirce;
    double m_orderBuySize;
    double m_orderSellPrice;
    double m_orderSellSize;
    double m_ocoOrderStaus;

public:
    cStrategyKingKeltner(void);
    ~cStrategyKingKeltner(void);
    virtual void onInit();
    virtual void onOrder(cOrderPtr);
    virtual void onTrade(CThostFtdcTradeField);
    virtual void onLoop(contextPtr);
    void sendOcoOrder(std::string inst,double upPrice, double downPrice, int fixedSize);
    void printStatus();
};

