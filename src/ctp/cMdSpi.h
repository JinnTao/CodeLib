#ifndef __CMDSPI_H__
#define __CMDSPI_H__

#include <ThostFtdcMdApi.h>
#include <ThostFtdcTraderApi.h>
#include <cString.h>
#include <memory>
#include "autotrade_config.h"
#include "common.h"
#include <functional>
#include <mutex>
using std::shared_ptr;

class cMarketDataCollection;
extern int iRequestID;

class cMdSpi : public CThostFtdcMdSpi {
public:
    // cMdSpi( CThostFtdcMdApi* pUserMdApi,  TThostFtdcBrokerIDType brokerID, TThostFtdcInvestorIDType investorID,
    // TThostFtdcPasswordType password, bool genLog = false );
    cMdSpi(){};
    ///����Ӧ��
    virtual void OnRspError(CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

    ///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
    ///@param nReason ����ԭ��
    ///        0x1001 �����ʧ��
    ///        0x1002 ����дʧ��
    ///        0x2001 ����������ʱ
    ///        0x2002 ��������ʧ��
    ///        0x2003 �յ�������
    virtual void OnFrontDisconnected(int nReason);

    ///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
    ///@param nTimeLapse �����ϴν��ձ��ĵ�ʱ��
    virtual void OnHeartBeatWarning(int nTimeLapse);

    ///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
    virtual void OnFrontConnected();

    ///��¼������Ӧ
    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin,
                                CThostFtdcRspInfoField*      pRspInfo,
                                int                          nRequestID,
                                bool                         bIsLast);

    ///��������Ӧ��
    virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument,
                                    CThostFtdcRspInfoField*            pRspInfo,
                                    int                                nRequestID,
                                    bool                               bIsLast);

    ///����ѯ��Ӧ��
    virtual void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField* pSpecificInstrument,
                                     CThostFtdcRspInfoField*            pRspInfo,
                                     int                                nRequestID,
                                     bool                               bIsLast);

    ///ȡ����������Ӧ��
    virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument,
                                      CThostFtdcRspInfoField*            pRspInfo,
                                      int                                nRequestID,
                                      bool                               bIsLast);

    ///ȡ������ѯ��Ӧ��
    virtual void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField* pSpecificInstrument,
                                       CThostFtdcRspInfoField*            pRspInfo,
                                       int                                nRequestID,
                                       bool                               bIsLast);

    ///�������֪ͨ
    virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData);

    ///ѯ��֪ͨ
    virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField* pForQuoteRsp);

    void RegisterMarketDataCollection(cMarketDataCollection* pMktDataCollection);

    bool getSatus() { return this->m_status; }

    void SubscribeMarketData(char* instIdList);
    void SubscribeMarketData(shared_ptr<vector<string>> instList);
    void SubscribeMarketData(string inst);


    void setOnFrontConnected(std::function<void()>&& fun);

    int32 init(const ctpConfig& ctp_config);
    int32 stop();
    int32 reConnect(const ctpConfig& ctp_config);

private:
    using CtpMdApiPtr = std::unique_ptr<CThostFtdcMdApi, std::function<void(CThostFtdcMdApi*)>>;
    void ReqUserLogin();

    void SubscribeForQuoteRsp();
    bool IsErrorRspInfo(CThostFtdcRspInfoField* pRspInfo);

    TThostFtdcBrokerIDType   m_brokerID;
    TThostFtdcInvestorIDType m_investorID;
    char                     m_password[252];

    CThostFtdcMdApi* m_pUserMdApi;

    cMarketDataCollection* m_pMktDataCollection;

    bool m_genLog;

    int     m_requestID;
    cString m_outputDirectory;
    cString m_logFileFolder;
    cString m_logFile;

    bool m_status;

    std::function<void()>       on_connected_fun_;
    std::mutex                  mut_;
    CtpMdApiPtr                 ctpmdapi_;
    std::promise<bool>          promise_result_;
    std::future<bool>           is_promised_result_;
};
typedef int (*ccbf_secureApi_LoginMd)(CThostFtdcMdApi*       ctp_futures_pMdApi,
                                      TThostFtdcBrokerIDType brokeId,
                                      TThostFtdcUserIDType   userId,
                                      char*                  pChar_passwd,
                                      int&                   ctp_futures_requestId);
#endif
