#ifndef __CSTRATEGY_H__
#define __CSTRATEGY_H__


#include <map>

#include <Windows.h>

#include <cTick.h>
#include <cCandle.h>
#include <cMarketDataCollection.h>
#include <cTradeCollection.h> 
#include <cPositionCollection.h>
#include <cOrderCollection.h>
#include "autotrade_config.h"
#include "cthread.h"
#include "ta_libc.h"
#include "cTraderSpi.h"
#include "cMdSpi.h"
#include "cDateTime.h"
#include "cSystem.h"
#include <thread>
#include <atomic>
class cTraderSpi;



class cStrategy
{
private:
	// non-copyable
	cStrategy( const cStrategy& ) {}
	cStrategy& operator = ( const cStrategy& ) {}

public:
	cStrategy();

	cStrategy(const string&);
	
	virtual ~cStrategy();

	virtual void init();

	virtual void unInit();

	void start();

	void stop();

	virtual void onOrder(cOrderPtr	){};

	virtual void onTrade(CThostFtdcTradeField ){};

	virtual void run(){cerr << this->m_strategyName << " runing" << endl;};


	virtual void setInst(string inst) { this->m_inst = inst; }
    virtual void setlots(int lots) { this->m_lots = lots; }
    virtual void setTimeMode(int timeMode) { this->m_timeMode = timeMode; }
    virtual void setCollectionName(string collectionName) { this->m_collectionName = collectionName; }
	virtual void setInitDate(string startDate, string endDate) { this->m_startDate = startDate; this->m_endDate = endDate; }

	virtual void sendStopOrder(string inst, DIRECTION inDirection,OFFSETFLAG inOffset, double price, UINT volume, string strategy,int slipNum = 1);

	virtual void processStopOrder(string inst, double lastData);

    virtual bool isTradeTime();
	// ***************************************************************************
	void RegisterMarketDataCollection( cMarketDataCollectionPtr p ){m_marketData = p;}
	void RegisterTradeSpi(cTraderSpi *p){m_pTradeSpi = p;}
	void RegisterMdSpi(cMdSpi *p){m_pMdSpi = p;}
	void RegisterPositionCollectionPtr(cPositionCollectionPtr p){m_pPositionC = p;};
	void RegisterOrderCollectionPtr(cOrderCollectionPtr p){m_pOrderC = p;}
	void RegisterTradeCollectionPtr(cTradeCollectionPtr p){m_pTradeC = p;}
	void RegisterTxtDir(string tradeDayDir, string oneMinuteDataDir){ m_tradeDayDir = tradeDayDir; m_oneMinuteDataDir = oneMinuteDataDir; }
	void RegisterAutoSetting(autoSetting *p) { this->m_pAutoSetting = p; }
//	void RegisterTradePlatForm(cTradingPlatform *p){m_pTradePlatform = p;}
protected:

    bool mode1();
    bool mode2();
    bool mode3();
    bool mode4();
    bool mode5();

	// base collection
	cMarketDataCollectionPtr m_marketData;
	// base mdptr tdptr
	cTraderSpi* m_pTradeSpi;
	cMdSpi* m_pMdSpi;

	cPositionCollectionPtr m_pPositionC;
	cOrderCollectionPtr m_pOrderC;
	cTradeCollectionPtr m_pTradeC;

	//stop Order List
	vector<cStopOrder> m_workingStopOrderList;
	
	//cTradingPlatform * m_pTradePlatform;

	// run status;
	bool m_status;

	string m_strategyName;

	int m_timeSpan;
	//txt database dir
	string m_tradeDayDir;
	string m_oneMinuteDataDir;

	string m_inst;
    int m_lots;
    int m_timeMode;
    string m_collectionName;

	string m_startDate;
	string m_endDate;

	autoSetting *m_pAutoSetting;



private:
	
	DWORD AutoTrading();
    std::thread m_thread;
    std::atomic<bool>    m_isRuning{ ATOMIC_FLAG_INIT };
	cThread< cStrategy >* m_pTradingThread;
};

typedef shared_ptr< cStrategy > cStrategyPtr;

#endif