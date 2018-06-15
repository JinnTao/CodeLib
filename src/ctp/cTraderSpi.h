#ifndef __CTRADERSPI_H__
#define __CTRADERSPI_H__

#include <map>
#include <yr_structs.h>
#include <cPositionCollection.h>
#include <cTradeCollection.h>
#include <cOrderCollection.h>
#include <cMdSpi.h>
#include <regex> // ����
#include <cMarketDataCollection.h>
#include "cStrategy.h"

using namespace std;
extern int iRequestID;
class cString;
template< class T > class cArray;

class cStrategy;

class cTraderSpi : public CThostFtdcTraderSpi
{
public:
    cTraderSpi() = default;
    cTraderSpi( CThostFtdcTraderApi* pUserTraderApi, cMdSpi* pUserMdSpi,CThostFtdcMdApi* pUserMdApi,TThostFtdcBrokerIDType brokerID, TThostFtdcInvestorIDType investorID, TThostFtdcPasswordType password, bool genLog = false );
    
    ~cTraderSpi();

    // After making a succeed connection with the CTP server, the client should send the login request to the CTP server.
    virtual void OnFrontConnected();

    // When the connection between client and the CTP server disconnected, the following function will be called
    virtual void OnFrontDisconnected(int nReason);

    // After receiving the login request from the client, the CTP server will send the following response to notify the client whether the login success or not
    virtual void OnRspUserLogin( CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

    // investor settlement information confirmation response
    virtual void OnRspSettlementInfoConfirm( CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

    // query instrument response
    virtual void OnRspQryInstrument( CThostFtdcInstrumentField* pInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

    // query trading account response
    virtual void OnRspQryTradingAccount( CThostFtdcTradingAccountField* pTradingAccount, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

    // After receiving the investor position request, the CTP server will send the following response to notify the client whether the request success or not
    //virtual void OnRspQryInvestorPosition( CThostFtdcInvestorPositionField* pInvestorPosition, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );
    
    // After receiving the investor position detail request, the CTP server will send the following response to notify the client whether the request success or not
    virtual void OnRspQryInvestorPositionDetail( CThostFtdcInvestorPositionDetailField* pInvestorPositionDetail, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

    // order insertion response
    virtual void OnRspOrderInsert( CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

    // order action response
    virtual void OnRspOrderAction( CThostFtdcInputOrderActionField* pInputOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ); 

    // order query response
    virtual void OnRspQryOrder( CThostFtdcOrderField* pOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );
    
    // the error notification caused by client request
    virtual void OnRspError( CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

    virtual void OnHeartBeatWarning( int nTimeLapse );

    virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);


    ///�����ѯ�ɽ���Ӧ
    virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    // order insertion return
    virtual void OnRtnOrder( CThostFtdcOrderField* pOrder );

    virtual void OnRtnTrade( CThostFtdcTradeField* pTrade);

    //�����ѯ��Լ����������Ӧ
    virtual void OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;


    void RegisterPositionCollection( cPositionCollectionPtr p );
    void RegisterOrderCollection( cOrderCollectionPtr p );
    void RegisterTradeCollection( cTradeCollectionPtr p );
    void RegisterSubscribeInstList(shared_ptr<vector<string>> p);
    void RegisterInstMessageMap( map<string, CThostFtdcInstrumentField*>* p );
    void RegisterInstCommissionMap(map<string,shared_ptr< CThostFtdcInstrumentCommissionRateField>> *p);
    void ReqQryInstrument();

    void ReqQryInstrument_all();

    void ReqQryTradingAccount();

    void ReqQryInvestorPosition_all();

    //void ReqQryInvestorPosition();
    void ReqQryInvestorPositionDetail();

    void ReqQryOrder();

    void ReqQryTrade();

    void ReqOrderInsert( cOrder* pOrder );

    void ReqOrderAction( shared_ptr<cOrder> pOrder );

    void ReqQryInstrumentCommissionRate(bool qryTrade = false);

    void Close();

    void Init();

    void GetInstrumentIDs( cArray< cString >& ) const;

    const sInstrumentInfo* GetInstrumentInfo( const cString& ) const;

    const sTradingAccountInfo* GetTradingAccountInfo() const { return m_accountInfo; }

    void saveInstrumentField(CThostFtdcInstrumentField* instField);

    void showPositionDetail();

    void ReqOrderInsert(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir, TThostFtdcCombOffsetFlagType kpp,TThostFtdcPriceType price,   TThostFtdcVolumeType vol);

    void insertOrder(string             inst,
                     traderTag::DIRECTION dire,
                     traderTag::OFFSETFLAG  flag,
                     int                vol,
                     double             orderPrice,
                     string             tag = "");

    void StraitClose(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir,TThostFtdcPriceType price,TThostFtdcVolumeType vol,string tag = "");

    char MapDirection(char src, bool toOrig);

    char MapOffset(char src, bool toOrig);

    bool subscribeInst(TThostFtdcInstrumentIDType instrumentName,bool tag);

    bool isValidInsturment(string inst,string& instName);


    void cancleAllPendingOrder();

    void cancleMyPendingOrder();

    void RegisterMarketDataEngine(cMarketDataCollectionPtr p){ this->m_pMarketDataEngine = p;}

    void RegisterStrategy(cStrategy *p) { m_strategyList.push_back(p); };
    
    int32 init(const ctpConfig& ctp_config);
    int32 stop();
    int32 reConnect(const ctpConfig& ctp_config);



private:
    CThostFtdcTraderApi* m_pUserTraderApi;
    cArray< cString > m_instrumentIDs;

    TThostFtdcOrderRefType    m_ORDER_REF;
    TThostFtdcFrontIDType    m_FRONT_ID;
    TThostFtdcSessionIDType    m_SESSION_ID;
    
    sTradingAccountInfo* m_accountInfo;
    map< cString, sInstrumentInfo* > m_instrumentInfo;        // useful trading information for traded instruments
    //
    /* postions */
    /*cPositionCollection* m_positionCollection;*/
    cPositionCollectionPtr m_positionCollection;
    // 
    /* orders */
    /*cOrderCollection* m_orderCollection;*/
    cOrderCollectionPtr m_orderCollection;

    vector<int> m_allOrderRef;                                            // list of all orderRef
    //
    /* trades */
    /*cTradeCollection* m_tradeCollection;*/
    cTradeCollectionPtr m_tradeCollection;

    //subscribe inst
    shared_ptr<vector<string>> m_pSubscribeInst;

    // Instrument detail Message Map    
    map<string, CThostFtdcInstrumentField*>* m_InstMeassageMap;

    // strategy List
    std::list<cStrategy*> m_strategyList;

    //
    map<string,shared_ptr<CThostFtdcInstrumentCommissionRateField>>*m_pInstCommissionMap;

    void ReqUserLogin();
    void ReqSettlementInfoConfirm();
    bool IsErrorRspInfo( CThostFtdcRspInfoField* pRspInfo );
    bool IsMyOrder( CThostFtdcOrderField* pOrder );

    TThostFtdcBrokerIDType    m_brokerID;
    TThostFtdcInvestorIDType m_investorID;
    char m_password[252];

    bool m_genLog;
    cString m_logFile;

    //=======================20170828==================
    bool m_first_inquiry_order;//�Ƿ��״β�ѯ����
    bool m_first_inquiry_trade;//�Ƿ��״β�ѯ�ɽ�
    bool m_firs_inquiry_Detail;//�Ƿ��״β�ѯ�ֲ���ϸ
    bool m_firs_inquiry_TradingAccount;//�Ƿ��״β�ѯ�ʽ��˺�
    bool m_firs_inquiry_Position;//�Ƿ��״β�ѯͶ���ֲ߳�
    bool m_first_inquiry_Instrument;//�Ƿ��״β�ѯ��Լ
    bool m_first_inquiry_commissionRate;//�Ƿ��״β�ѯ������
    
    vector<CThostFtdcOrderField*> m_orderList;//ί�м�¼��ȫ����Լ
    vector<CThostFtdcOrderField*> m_pendOrderList;//�ҵ���¼��ȫ����Լ
    vector<CThostFtdcTradeField*> m_tradeList;//�ɽ���¼��ȫ����Լ

    vector<CThostFtdcTradeField*> m_tradeListNotClosedAccount;//δƽ�ּ�¼

    map<string,cPositionDetailPtr> m_position_message_map;//�ֲּ�¼ 

    double m_closeProfit;//ƽ��ӯ�������к�Լһ������ֵ��������m_trade_message_map�е�������ÿ����Լ��ƽ��ӯ��ֵ
    
    double m_OpenProfit;//����ӯ�������к�Լһ������ֵ��������m_trade_message_map�е�������ÿ����Լ�ĸ���ӯ��ֵ

    //map<string, CThostFtdcInstrumentField*> m_instMessage_map;//�����Լ��Ϣ��map
    
    cMdSpi* m_pMdSpi;//����APIָ�룬���캯���︳ֵ

    CThostFtdcMdApi* m_pMDUserApi_td;
    double m_accountMargin;

    fstream m_output;
    string m_tradeDay;
    string m_actionDay;
    bool m_qryStatus;
    ///
    map<string,CThostFtdcInstrumentField*>::iterator m_itMap;// ���ڲ�ѯ��Լ
    /// marketData
    cMarketDataCollectionPtr m_pMarketDataEngine;
};

typedef int (*ccbf_secureApi_LoginTrader)(CThostFtdcTraderApi* ctp_futures_pTraderApi, TThostFtdcBrokerIDType brokeId, TThostFtdcUserIDType userId, char* pChar_passwd, int& ctp_futures_requestId);

#endif

