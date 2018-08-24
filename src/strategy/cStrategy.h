#ifndef __CSTRATEGY_H__
#define __CSTRATEGY_H__

#include <map>
#include <thread>
#include <atomic>

#include "ta_libc.h"

#include "cMarketDataCollection.h"
#include "cTradeCollection.h"
#include "cPositionCollection.h"
#include "cOrderCollection.h"

#include "cTraderSpi.h"
#include "cMdSpi.h"

#include "logger.h"
#include "common.h"
#include "global.h"

class cTraderSpi;

typedef unsigned int DateTimeFormat;

class cStrategy {
private:
    // non-copyable
    cStrategy(const cStrategy&) {}
    cStrategy& operator=(const cStrategy&) {}

public:
    cStrategy();

    cStrategy(const string&);

    virtual ~cStrategy();

    virtual void init();

    virtual void unInit();

    void start();

    void stop();

    virtual void onOrder(cOrderPtr){};

    virtual void onTrade(CThostFtdcTradeField){};

    virtual void run() { cerr << this->m_strategyName << " runing" << endl; };

    virtual void setInst(string inst) { this->m_inst = inst; }
    virtual void setlots(int lots) { this->m_lots = lots; }
    virtual void setTimeMode(int timeMode) { this->m_timeMode = timeMode; }
    virtual void setCollectionName(string collectionName) { this->m_collectionName = collectionName; }
    virtual void setInitDate(string startDate, string endDate) {
        this->m_startDate = startDate;
        this->m_endDate   = endDate;
    }

    virtual void sendStopOrder(string                inst,
                               traderTag::DIRECTION  inDirection,
                               traderTag::OFFSETFLAG inOffset,
                               double                price,
                               int                   volume,
                               string                strategy,
                               int                   slipNum = 1);

    virtual void processStopOrder(string inst, double lastData);

    virtual bool isTradeTime();
    // ***************************************************************************
    void RegisterMarketDataCollection(cMarketDataCollectionPtr p) { m_marketData = p; }
    void RegisterTradeSpi(std::shared_ptr<cTraderSpi> p) { m_pTradeSpi = p; }
    void RegisterMdSpi(std::shared_ptr<cMdSpi> p) { m_pMdSpi = p; }
    void RegisterPositionCollectionPtr(cPositionCollectionPtr p) { m_pPositionC = p; };
    void RegisterOrderCollectionPtr(cOrderCollectionPtr p) { m_pOrderC = p; }
    void RegisterTradeCollectionPtr(cTradeCollectionPtr p) { m_pTradeC = p; }


    void RegisterTxtDir(string tradeDayDir, string oneMinuteDataDir) {
        m_tradeDayDir      = tradeDayDir;
        m_oneMinuteDataDir = oneMinuteDataDir;
    }

    void RegisterAutoSetting(strategyConfig* p) { this->m_pAutoSetting = p; }
    bool GetStrategyStatus() { return m_isRuning; };
    //    void RegisterTradePlatForm(cTradingPlatform *p){m_pTradePlatform = p;}
protected:
    bool mode1(DateTimeFormat hourMinTime);
    bool mode2(DateTimeFormat hourMinTime);
    bool mode3(DateTimeFormat hourMinTime);
    bool mode4(DateTimeFormat hourMinTime);
    bool mode5(DateTimeFormat hourMinTime);
    tm*   getLocalNowTm();
    // base collection
    cMarketDataCollectionPtr m_marketData;
    // base mdptr tdptr
    shared_ptr<cTraderSpi> m_pTradeSpi;
    shared_ptr<cMdSpi>     m_pMdSpi;

    cPositionCollectionPtr m_pPositionC;
    cOrderCollectionPtr    m_pOrderC;
    cTradeCollectionPtr    m_pTradeC;

    // stop Order List
    vector<cStopOrder> m_workingStopOrderList;

    // cTradingPlatform * m_pTradePlatform;

    // run status;
    bool m_status;

    string m_strategyName;

    int m_timeSpan;
    // txt database dir
    string m_tradeDayDir;
    string m_oneMinuteDataDir;

    string m_inst;
    int    m_lots;
    int    m_timeMode;
    string m_collectionName;

    string m_startDate;
    string m_endDate;

    strategyConfig* m_pAutoSetting;

    string strategy_id_name_;

private:
    int               AutoTrading();
    std::thread         m_thread;
    std::atomic<bool>   m_isRuning{ATOMIC_FLAG_INIT};
};

typedef shared_ptr<cStrategy> cStrategyPtr;

#endif