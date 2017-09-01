#ifndef __CMDSPI_H__
#define __CMDSPI_H__

#include <stl_ctp.h>
#include <cString.h>

template< class T > class cArray;
class cMarketDataCollection;

class cMdSpi : public CThostFtdcMdSpi
{
public:
	cMdSpi( CThostFtdcMdApi* pUserMdApi,  TThostFtdcBrokerIDType brokerID, TThostFtdcInvestorIDType investorID, TThostFtdcPasswordType password, bool genLog = false );

	///����Ӧ��
	virtual void OnRspError( CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );

	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	///@param nReason ����ԭ��
	///        0x1001 �����ʧ��
	///        0x1002 ����дʧ��
	///        0x2001 ����������ʱ
	///        0x2002 ��������ʧ��
	///        0x2003 �յ�������
	virtual void OnFrontDisconnected( int nReason );
		
	///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	///@param nTimeLapse �����ϴν��ձ��ĵ�ʱ��
	virtual void OnHeartBeatWarning( int nTimeLapse );

	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();
	
	///��¼������Ӧ
	virtual void OnRspUserLogin( CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

	///��������Ӧ��
	virtual void OnRspSubMarketData( CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

	///����ѯ��Ӧ��
	virtual void OnRspSubForQuoteRsp( CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

	///ȡ����������Ӧ��
	virtual void OnRspUnSubMarketData( CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

	///ȡ������ѯ��Ӧ��
	virtual void OnRspUnSubForQuoteRsp( CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

	///�������֪ͨ
	virtual void OnRtnDepthMarketData( CThostFtdcDepthMarketDataField* pDepthMarketData );

	///ѯ��֪ͨ
	virtual void OnRtnForQuoteRsp( CThostFtdcForQuoteRspField* pForQuoteRsp );

	void RegisterMarketDataCollection( cMarketDataCollection* pMktDataCollection );

	bool getSatus(){return this->m_status;}
private:
	void ReqUserLogin();
	void SubscribeMarketData(char *instIdList);
	void SubscribeForQuoteRsp();
	bool IsErrorRspInfo( CThostFtdcRspInfoField* pRspInfo );

	TThostFtdcBrokerIDType	m_brokerID;
	TThostFtdcInvestorIDType m_investorID;
	TThostFtdcPasswordType	m_password;

	CThostFtdcMdApi* m_pUserMdApi;

	cMarketDataCollection* m_pMktDataCollection;
	
	cArray< cString > m_instrumentIDs;

	bool m_genLog;
	
	int m_requestID;
	cString m_outputDirectory;
	cString m_logFileFolder;
	cString m_logFile;

	bool m_status;

};

#endif
