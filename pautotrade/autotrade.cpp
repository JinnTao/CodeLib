#include <autotrade.h>
#include <cStrategyMovAvg.h>
#include <autotrade_config.h>

//#pragma comment(lib,"json_vc71_libmtd.lib")

HANDLE g_hEvent;//�¼����

int iRequestID = 0;//�������

int main()
{

	autotrade_trade();

	return 0;
	
}

void autotrade_trade()
{
	try
	{
		printf( "\n" );
		printf( "running process to automatically trade with self-defined strategies...\n" );
		
		//-------------------------------------��ȡ��������---------------------------------------
		AccountParam ctpAccount;
		mongoSetting mongoDbSetting;
		ParseSettingJson(ctpAccount,mongoDbSetting);
		

		//-------------------------------------����ǰ�ýӿ�--------------------------------------
		/* MDApi & MDSpi */
		CThostFtdcMdApi* pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi(".\\MDflow\\");// �����ļ� ��ֹ����
		cMdSpi* pMdUserSpi = new cMdSpi( pMdUserApi, ctpAccount.brokerId, ctpAccount.userId, ctpAccount.passwd );
		pMdUserApi->RegisterSpi( pMdUserSpi );
		pMdUserApi->RegisterFront( ctpAccount.mdAddress );

		//-------------------------------------���������ռ���--------------------------------------------
		/* cMarketDataCollection */
		cMarketDataCollectionPtr pMdEngine = make_shared< cMarketDataCollection >();
		dynamic_cast< cMdSpi* >( pMdUserSpi )->RegisterMarketDataCollection( pMdEngine.get() );
		//
		/* Strategy */
		
		cStrategyPtr pStrategy = make_shared< cStrategyMovAvg >( "MovAvg" );
		pStrategy->RegisterMarketDataCollection( pMdEngine );

		//
		/* TraderApi && TraderSpi */
		CThostFtdcTraderApi* pTraderUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(".\\TDflow\\");
		cTraderSpi* pTraderUserSpi = new cTraderSpi( pTraderUserApi,pMdUserSpi,pMdUserApi, ctpAccount.brokerId, ctpAccount.userId, ctpAccount.passwd );
		pTraderUserApi->RegisterSpi((CThostFtdcTraderSpi*) pTraderUserSpi );
		pTraderUserApi->SubscribePublicTopic( THOST_TERT_RESTART );	// subsribe public topic
		pTraderUserApi->SubscribePrivateTopic( THOST_TERT_QUICK );	// subscribe private topic
		pTraderUserApi->RegisterFront( ctpAccount.tdAddress ); 

		//-----------------------------------------�˻������߳�---------------------------------------------------------------------------------
		cTradingPlatformPtr pTradingPlatform = make_shared< cTradingPlatform >();
		pTradingPlatform->RegisterMarketDataEngine( pMdEngine );
		pTradingPlatform->RegisterTraderSpi(  pTraderUserSpi);
		pTradingPlatform->RegisterMdSpi(pMdUserSpi);
		pTradingPlatform->RegisterStrategy( pStrategy );

		cThread< cTradingPlatform >* pTradingThread = new cThread< cTradingPlatform >( pTradingPlatform.get(), &cTradingPlatform::AutoTrading );



		pTraderUserApi->Init(); //Ӧ���ȳ�ʼ��TD �ٽ���MD��ʼ�� ����ʼ�� ����ƽ̨ ����˼��Ϊ��ȡ���˻���Ϣ�� ����������
		

		while(true){
			if(pMdUserSpi->getSatus()){
				pTradingThread->Init();
				break;
			}
		} // ��ֹJoin�޷�ͣס�����߳��˳�

		//�ȴ��߳��˳�
		pTraderUserApi->Join();
		while(true){}
	}
	catch( const char* err )
	{
		printf( "\n%s\n", err );
		exit( 1 );
	}
}