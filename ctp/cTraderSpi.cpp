#include <cSystem.h>
#include <cTraderSpi.h>
#include <global_defs.h>
#include <cTrade.h>
extern HANDLE g_hEvent;
bool IsFlowControl( int iResult )
{
	return (( iResult == -2 ) || ( iResult == -3 ));
}

cTraderSpi::cTraderSpi( CThostFtdcTraderApi* pUserTraderApi,CThostFtdcMdApi* pUserMdApi,TThostFtdcBrokerIDType brokerID, TThostFtdcInvestorIDType investorID, TThostFtdcPasswordType password, bool genLog )
: m_pUserTraderApi( pUserTraderApi )
, m_genLog( genLog )
{
	strcpy_s( m_brokerID, sizeof( TThostFtdcBrokerIDType ), brokerID );
	strcpy_s( m_investorID, sizeof( TThostFtdcInvestorIDType ), investorID );
	strcpy_s( m_password, sizeof( TThostFtdcPasswordType ), password );
	this->m_pMDUserApi_td = pUserMdApi;

	m_first_inquiry_order = true;//�Ƿ��״β�ѯ����
	m_first_inquiry_trade = true;//�Ƿ��״β�ѯ�ɽ�
	m_firs_inquiry_Detail = true;//�Ƿ��״β�ѯ�ֲ���ϸ
	m_firs_inquiry_TradingAccount = true;//�Ƿ��״β�ѯ�ʽ��˺�
	m_firs_inquiry_Position = true;//�Ƿ��״β�ѯͶ���ֲ߳�
	m_first_inquiry_Instrument = true;//�Ƿ��״β�ѯ��Լ

	m_closeProfit = 0.0;//ƽ��ӯ��
	m_OpenProfit = 0.0;//����ӯ��

	m_accountMargin = 0.0;

	m_InstMeassageMap = NULL;
}

cTraderSpi::~cTraderSpi()
{
	delete m_accountInfo;
	
}

// After making a succeed connection with the CTP server, the client should send the login request to the CTP server.
void cTraderSpi::OnFrontConnected()
{
	char message[256];
	sprintf( message, "%s:called cTraderSpi::OnFrontConnected.", cSystem::GetCurrentTimeBuffer().c_str() );
	cout << message << endl;

	// log info
	if( m_genLog )
		cSystem::WriteLogFile( m_logFile.c_str(), message, false );

	ReqUserLogin();
	SetEvent(g_hEvent);
}

void cTraderSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset( &req, 0, sizeof( req ) );
	strcpy_s( req.BrokerID, sizeof( TThostFtdcBrokerIDType ), m_brokerID );
	strcpy_s( req.UserID, sizeof( TThostFtdcInvestorIDType ), m_investorID );
	strcpy_s( req.Password, sizeof( TThostFtdcPasswordType ), m_password );

	int iResult = m_pUserTraderApi->ReqUserLogin( &req, ++iRequestID );
	

	char message[256];
	sprintf( message, "%s:called cTraderSpi::ReqUserLogin: %s.", cSystem::GetCurrentTimeBuffer().c_str(), ( ( iResult == 0 ) ? "Success" : "Fail") );
	cout << message << endl;

}

// When the connection between client and the CTP server disconnected, the following function will be called
void cTraderSpi::OnFrontDisconnected(int nReason)
{
	// in this case, API will reconnect, the client application can ignore this
	char message[256];
	sprintf( message, "%s:called cTraderSpi::OnFrontDisconnected. Reason = %d", cSystem::GetCurrentTimeBuffer().c_str(), nReason );
	cout << message << endl;
	
}

// After receiving the login request from the client, the CTP server will send the following response to notify the client whether the login success or not
void cTraderSpi::OnRspUserLogin( CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast )
{
	if( !IsErrorRspInfo( pRspInfo )&&pRspUserLogin )
	{
		m_FRONT_ID = pRspUserLogin->FrontID;
		m_SESSION_ID = pRspUserLogin->SessionID;
		int iNextOrderRef = atoi( pRspUserLogin->MaxOrderRef );

		iNextOrderRef++;
		sprintf( m_ORDER_REF, "%d", iNextOrderRef );


		char message[256];
		sprintf( message, "%s:called cTraderSpi::OnRspUserLogin.", cSystem::GetCurrentTimeBuffer().c_str() );
		cout << message << endl;

		ReqSettlementInfoConfirm();
	}
	if(bIsLast) SetEvent(g_hEvent);
}

void cTraderSpi::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset( &req, 0, sizeof( req ) );
	strcpy( req.BrokerID, m_brokerID );
	strcpy( req.InvestorID, m_investorID );
	int iResult = m_pUserTraderApi->ReqSettlementInfoConfirm( &req, ++iRequestID );


	char message[256];
	sprintf( message, "%s:called cTraderSpi::ReqSettlementInfoConfirm: %s.", cSystem::GetCurrentTimeBuffer().c_str(), ( ( iResult == 0 ) ? "Success" : "Fail") );
	cout << message << endl;
}



void cTraderSpi::OnRspSettlementInfoConfirm( CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast )
{
	if(!IsErrorRspInfo( pRspInfo ) && pSettlementInfoConfirm )
	{
		char message[256];
		sprintf( message, "%s:called cTraderSpi::OnRspSettlementInfoConfirm.Success ", cSystem::GetCurrentTimeBuffer().c_str() );
		cout << message << endl;
		Sleep(1000);// wait CTP have enough time to response
		//ReqQryInstrument();
		ReqQryOrder();
	}
	if(bIsLast)SetEvent(g_hEvent);
}


void cTraderSpi::ReqQryOrder()
{
	CThostFtdcQryOrderField req;
	memset(&req, 0, sizeof(req));

	strcpy(req.InvestorID, this->m_investorID);//Ͷ���ߴ���

	int iResult = m_pUserTraderApi->ReqQryOrder(&req, ++iRequestID);

	char message[256];
	sprintf( message, "%s:called cTraderSpi::ReqQryOrder: %s.", cSystem::GetCurrentTimeBuffer().c_str(), ( ( iResult == 0 ) ? "Success" : "Fail") );
	cout << message << endl;

}


void cTraderSpi::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	

	if (!IsErrorRspInfo(pRspInfo) && pOrder )
	{
		//cerr<<"OnRspQryOrder: FrontID:"<<pOrder->FrontID<<"  SessionID:"<<pOrder->SessionID<<" OrderRef:"<<pOrder->OrderRef<<endl;

		if(m_first_inquiry_order == true)
		{
			CThostFtdcOrderField* order = new CThostFtdcOrderField();
			memcpy(order,  pOrder, sizeof(CThostFtdcOrderField));
			m_orderList.push_back(order);

			if(pOrder->OrderStatus == '1'  || pOrder->OrderStatus == '3'){
				CThostFtdcOrderField* pendOrder = new CThostFtdcOrderField();
				memcpy(pendOrder,  pOrder, sizeof(CThostFtdcOrderField));
				this->m_pendOrderList.push_back(pendOrder);
			}

			if(bIsLast) 
			{
				m_first_inquiry_order = false;
				char message[256];
				sprintf( message, "%s:called cTraderSpi::OnRspQryOrder success.", cSystem::GetCurrentTimeBuffer().c_str());
				cout << message << endl;
				cerr<<"All order��" << m_orderList.size()<<endl;

				cerr<<"--------------------------------------------------------------------order start"<<endl;
				for(vector<CThostFtdcOrderField*>::iterator iter = m_orderList.begin(); iter != m_orderList.end(); iter++)
					//cerr<<"BrokerId:"<<(*iter)->BrokerID<<endl<<" InvestorId:"<<(*iter)->InvestorID<<endl<<" �û�����:"<<(*iter)->UserID<<endl<<" ��Լ����:"<<(*iter)->InstrumentID<<endl<<" ��������:"<<(*iter)->Direction<<endl
					//<<" ��Ͽ�ƽ��־:"<<(*iter)->CombOffsetFlag<<endl<<" �۸�:"<<(*iter)->LimitPrice<<endl<<" ����:"<<(*iter)->VolumeTotalOriginal<<endl<<" ��������:"<< (*iter)->OrderRef<<endl<<" �ͻ�����:"<<(*iter)->ClientID<<endl
					//<<" ����״̬:"<<(*iter)->OrderStatus<<endl<<" ί��ʱ��:"<<(*iter)->InsertTime<<endl<<" �������:"<<(*iter)->OrderSysID<<endl<<" GTD����:"<<(*iter)->GTDDate<<endl<<" ������:"<<(*iter)->TradingDay<<endl
					//<<" ��������:"<<(*iter)->InsertDate<<endl
					//<<endl;
					cerr<<" UserId:"<<(*iter)->UserID<<" Inst:"<<(*iter)->InstrumentID<<" Dire:"<<(*iter)->Direction<<" price:"<<(*iter)->LimitPrice<<" lots:"<<(*iter)->VolumeTotalOriginal<<
					" status:"<<(*iter)->OrderStatus<<" insetTime:"<<(*iter)->InsertTime<<"  insertDate:"<<(*iter)->InsertDate<<endl
					<<endl;
				cerr<<"--------------------------------------------------------------------order end"<<endl;


				Sleep(1000);
				cerr<<"First Qry Order Success"<<endl;
				ReqQryInvestorPositionDetail();

				SetEvent(g_hEvent);

			}
		}


	}
	else
	{
		if(m_first_inquiry_order == true ) 
		{
			m_first_inquiry_order = false;
			Sleep(1000);
			cerr<<"Error or no order"<<endl;
			ReqQryInvestorPositionDetail();
		}

	}

	//cerr<<"-----------------------------------------------First Request Qry Order Over"<<endl;

}

 
//request Query Investor posistion Detail
void cTraderSpi::ReqQryInvestorPositionDetail()
{
	CThostFtdcQryInvestorPositionDetailField req;
	memset(&req, 0, sizeof(req));

	strcpy(req.InvestorID, m_investorID);//investor Id

	int iResult = m_pUserTraderApi->ReqQryInvestorPositionDetail(&req, ++iRequestID);

	char message[256];
	sprintf( message, "%s:called cTraderSpi::ReqQryInvestorPositionDetail: %s.",  cSystem::GetCurrentTimeBuffer().c_str(), ( ( iResult == 0 ) ? "Success" : "Fail") );
	cout << message << endl;
}

///Request Query Investor position Detail,First Query & followed Query
void cTraderSpi::OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//cerr<<"OnRspQryInvestorPositionDetail"<<endl;

	if(!IsErrorRspInfo(pRspInfo) && pInvestorPositionDetail )
	{

		//all Instrument
		if(m_firs_inquiry_Detail == true)
		{	
			//�������к�Լ����������ƽ�ֵģ�ֻ����δƽ�ֵ�
			//Ϊʲô���ڲ�ѯ�ɽ��ص�����OnRspQryTrade()������,��Ϊֻ�ܲ�ѯ�����
			//ʹ�ýṹ����CThostFtdcTradeField����Ϊ����ʱ���ֶΣ���CThostFtdcInvestorPositionDetailFieldû��ʱ���ֶ�

			CThostFtdcTradeField* trade = new CThostFtdcTradeField();

			strcpy_s(trade->InvestorID,sizeof(TThostFtdcInvestorIDType), pInvestorPositionDetail->InvestorID);///InvestorID
			strcpy_s(trade->InstrumentID,sizeof(TThostFtdcInstrumentIDType), pInvestorPositionDetail->InstrumentID);///InstrumentID
			strcpy_s(trade->ExchangeID,sizeof(TThostFtdcExchangeIDType), pInvestorPositionDetail->ExchangeID);///ExchangeID
			trade->Direction = pInvestorPositionDetail->Direction;///Direction
			trade->Price = pInvestorPositionDetail->OpenPrice;///OpenPrice
			trade->Volume = pInvestorPositionDetail->Volume;///Volume
			strcpy_s(trade->TradeDate, sizeof(TThostFtdcDateType),pInvestorPositionDetail->OpenDate);///OpenDate


			if(pInvestorPositionDetail->Volume > 0)//ɸѡδƽ�ֵ�
			{
				m_tradeListNotClosedAccount.push_back(trade);
			}


			//������к�Լ�ĳֲ���ϸ��Ҫ����߽�����һ���Ĳ�ѯReqQryTradingAccount();
			if(bIsLast)
			{
				char message[256];
				sprintf( message, "%s:called cTraderSpi::OnRspQryInvestorPositionDetail success.", cSystem::GetCurrentTimeBuffer().c_str());
				cout << message << endl;

				cerr<<"All order��" << m_orderList.size()<<endl;
				m_firs_inquiry_Detail = false;

				cerr<<"----------------------------------------------Position Detail start"<<endl;

				for(vector<CThostFtdcTradeField*>::iterator iter = m_tradeListNotClosedAccount.begin(); iter != m_tradeListNotClosedAccount.end(); iter++)
				{
					cerr<<"InvestorID:"<<(*iter)->InvestorID
						<<" Inst:"<<(*iter)->InstrumentID
						<<" Exchange:"<<(*iter)->ExchangeID
						<<" Dire:"<<((*iter)->Direction == '0'? "long" : "short")
						<<" Price:"<<(*iter)->Price
						<<" vol:"<<(*iter)->Volume
						<<" tradeDate:"<<(*iter)->TradeDate
						<<" tradeTime:"<<(*iter)->TradeTime<<endl;
				}

				cerr<<"----------------------------------------------Position Detail end"<<endl;


				Sleep(1000);
				ReqQryTradingAccount();

			}
		}



	}
	else
	{
		if(m_firs_inquiry_Detail == true)
		{
			m_firs_inquiry_Detail = false;
			Sleep(1000);
		
			ReqQryTradingAccount();
		}

	}

	//cerr<<"-----------------------------------------------OnRspQryInvestorPositionDetail End"<<endl;

}

void cTraderSpi::ReqQryTradingAccount()
{
	CThostFtdcQryTradingAccountField req;
	memset( &req, 0, sizeof( req ) );
	strcpy( req.BrokerID, m_brokerID );
	strcpy( req.InvestorID, m_investorID );
	int iResult = m_pUserTraderApi->ReqQryTradingAccount( &req, ++iRequestID );
	char message[256];
	sprintf( message, "%s:called cTraderSpi::ReqQryTradingAccount: %s.", cSystem::GetCurrentTimeBuffer().c_str(), ( ( iResult == 0 ) ? "Success" : "Fail") );
	cout << message << endl;
	
}

void cTraderSpi::OnRspQryTradingAccount( CThostFtdcTradingAccountField* pTradingAccount, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast )
{
	//cerr<<"OnRspQryTradingAccount"<<endl;

	if (!IsErrorRspInfo(pRspInfo) &&  pTradingAccount)
	{

		m_accountInfo = new sTradingAccountInfo;
		memset( m_accountInfo, 0, sizeof( sTradingAccountInfo ) );
		strcpy( m_accountInfo->BrokerID, pTradingAccount->BrokerID );
		strcpy( m_accountInfo->AccountID, pTradingAccount->AccountID );
		m_accountInfo->PreBalance = pTradingAccount->PreBalance;
		m_accountInfo->Balance = pTradingAccount->Balance;
		m_accountInfo->Available = pTradingAccount->Available;
		m_accountInfo->WithdrawQuota = pTradingAccount->WithdrawQuota;
		m_accountInfo->CurrMargin = pTradingAccount->CurrMargin;
		m_accountInfo->CloseProfit = pTradingAccount->CloseProfit;
		m_accountInfo->PositionProfit = pTradingAccount->PositionProfit;
		m_accountInfo->Commission = pTradingAccount->Commission;
		m_accountInfo->FrozenMargin = pTradingAccount->FrozenMargin;

		printf("\nAccount Summary:\n");
		printf("   AccountID:%s\n", m_accountInfo->AccountID );
		printf("   PreBalance:%f\n", m_accountInfo->PreBalance );
		printf("   Balance:%f\n", m_accountInfo->Balance );
		printf("   Available:%f\n", m_accountInfo->Available );
		printf("   WithdrawQuota:%f\n", m_accountInfo->WithdrawQuota );
		printf("   CurrMargin:%f\n", m_accountInfo->CurrMargin );
		printf("   CloseProfit:%f\n", m_accountInfo->CloseProfit );
		printf("   PositionProfit:%f\n", m_accountInfo->PositionProfit );
		printf("   Commission:%f\n", m_accountInfo->Commission );
		printf("   FrozenMargin:%f\n", m_accountInfo->FrozenMargin );

		if(m_firs_inquiry_TradingAccount == true)
		{
			m_firs_inquiry_TradingAccount = false;
			Sleep(1000);

			cerr<<"OnRspQryTradingAccount success"<<endl;		
			
			ReqQryInvestorPosition_all();
		}


	}

	else
	{
		if(m_firs_inquiry_TradingAccount == true)
		{
			m_firs_inquiry_TradingAccount = false;
			Sleep(1000);
			
			cerr<<"OnRspQryTradingAccount error"<<endl;			
			
			ReqQryInvestorPosition_all();
			
		}

	}
	if(bIsLast) SetEvent(g_hEvent);

//	cerr<<"-----------------------------------------------OnRspQryTradingAccount End"<<endl;
}
///
void cTraderSpi::ReqQryInvestorPosition_all()
{
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	//strcpy(req.BrokerID, appId);
	//strcpy(req.InvestorID, userId);
	//strcpy(req.InstrumentID, instId);

	int iResult = m_pUserTraderApi->ReqQryInvestorPosition(&req, ++iRequestID);
	char message[256];
	sprintf( message, "%s:called cTraderSpi::ReqQryInvestorPosition: %s.", cSystem::GetCurrentTimeBuffer().c_str(), ( ( iResult == 0 ) ? "Success" : "Fail" ) );
	cout << message << endl;
}

//�ֲֲ�ѯ�ص�����,�Ѿ�ƽ�ֵĵ��ӣ��ֲ���Ϊ0�˻��᷵��
void cTraderSpi::OnRspQryInvestorPosition(
	CThostFtdcInvestorPositionField *pInvestorPosition, 
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//cerr<<"OnRspQryInvestorPosition"<<endl;

	if( !IsErrorRspInfo(pRspInfo) &&  pInvestorPosition)
	{
		//�˺������к�Լ
		if(m_firs_inquiry_Position == true)
		{

			this->m_positionCollection->update(pInvestorPosition);

			if (bIsLast)
			{
				m_firs_inquiry_Position = false;

				this->m_positionCollection->PrintDetail();

				Sleep(1000);
				ReqQryInstrument_all();

			}

		}



	}
	else
	{
		if(m_firs_inquiry_Position == true)
		{
			m_firs_inquiry_Position = false;

			cerr<<"OnRspQryInvestorPosition Error or No position"<<endl;
			Sleep(1000);
			ReqQryInstrument_all();
		}

	}

	//cerr<<"-----------------------------------------------��ѯ�ֲַ��ص��ν���"<<endl;
}


///�����ѯ��Լ��Ϣ�����к�Լ
void cTraderSpi::ReqQryInstrument_all()
{
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));

	int iResult = m_pUserTraderApi->ReqQryInstrument(&req, ++iRequestID);
	char message[256];
	sprintf( message, "%s:called cTraderSpi::ReqQryInstrument: %s.", cSystem::GetCurrentTimeBuffer().c_str(), ( ( iResult == 0 ) ? "Success" : "Fail" ) );
	cout << message << endl;
}

void cTraderSpi::OnRspQryInstrument( CThostFtdcInstrumentField* pInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast )
{
	
	if ( !IsErrorRspInfo(pRspInfo) &&  pInstrument)
	{
		if(m_first_inquiry_Instrument == true)
		{
			//�������к�Լ����Ϣ��map
			CThostFtdcInstrumentField* instField = new CThostFtdcInstrumentField();
			memcpy(instField,pInstrument, sizeof(CThostFtdcInstrumentField));
			m_InstMeassageMap->insert(pair<string, CThostFtdcInstrumentField*> (instField->InstrumentID, instField));


			saveInstrumentField(pInstrument);

			if(bIsLast)
			{
				m_first_inquiry_Instrument = false;


				cout<<"Trade Init Finish, start up MD:"<<endl;

				m_pMDUserApi_td->Init();

				SetEvent(g_hEvent);

			}

		}



	}


	//// copyright Yiran Yang
	//for (int i = 0; i < m_instrumentIDs.getSize(); ++i)
	//{
	//	if( Compare( pInstrument->InstrumentID, m_instrumentIDs[i] ) )
	//	{
	//		
	//		map< cString, sInstrumentInfo* >::iterator it = m_instrumentInfo.find( m_instrumentIDs[i] );
	//		if( it == m_instrumentInfo.end() )
	//		{
	//			sInstrumentInfo* info = new sInstrumentInfo;
	//			memset( info, 0, sizeof( sInstrumentInfo ) );
	//			strcpy( info->InstrumentID, pInstrument->InstrumentID );
	//			strcpy( info->ExchangeID, pInstrument->ExchangeID );
	//			strcpy( info->ProductID, pInstrument->ProductID );
	//			info->MaxMarketOrderVolume = pInstrument->MaxMarketOrderVolume;
	//			info->MinMarketOrderVolume = pInstrument->MinMarketOrderVolume;
	//			info->MaxLimitOrderVolume = pInstrument->MaxLimitOrderVolume;
	//			info->MinLimitOrderVolume = pInstrument->MinLimitOrderVolume;
	//			info->VolumeMultiple = pInstrument->VolumeMultiple;
	//			info->PriceTick = pInstrument->PriceTick;
	//			info->IsTrading = pInstrument->IsTrading;
	//			info->LongMarginRatio = pInstrument->LongMarginRatio;
	//			info->ShortMarginRatio = pInstrument->ShortMarginRatio;
	//			m_instrumentInfo.insert( pair< cString, sInstrumentInfo* >( m_instrumentIDs[i], info ) );
	//		}
	//		break;
	//	}
	//}

	//if( bIsLast && !IsErrorRspInfo( pRspInfo ) )
	//{
	//	printf( "\nInstrument List:\n" );
	//	for( map< cString, sInstrumentInfo* >::iterator it = m_instrumentInfo.begin(); it != m_instrumentInfo.end(); ++it )
	//	{
	//		printf("   Instrument:%s Exchange:%s\n", (*it).second->InstrumentID, (*it).second->ExchangeID );
	//	}

	//	if( m_genLog )
	//	{
	//		char message[256];
	//		sprintf( message, "%s:called cTraderSpi::OnRspQryInstrument.", cSystem::GetCurrentTimeBuffer().c_str() );
	//		cout << message << endl;
	//		cSystem::WriteLogFile( m_logFile.c_str(), message, false );
	//	}
	//	ReqQryTradingAccount();
	//}
}

void cTraderSpi::saveInstrumentField(CThostFtdcInstrumentField* instField){
	// ���� ���Ǳ��浽���ݿ���
	//string idate = 
	ofstream output;
	output.open("output/instrumentListField.csv",ios::_Nocreate | ios::ate) ;
	if(output){
		//content
		output  << instField->InstrumentID << "," 
				<< instField->ExchangeID << "," 
				<< instField->InstrumentName << "," 
				<< instField->ExchangeInstID << "," 
				<< instField->ProductID << "," 
				<< instField->ProductClass << "," 
				<< instField->DeliveryYear << "," 
				<< instField->DeliveryMonth << "," 
				<< instField->MaxMarketOrderVolume << "," 
				<< instField->MinMarketOrderVolume << "," 
				<< instField->MaxLimitOrderVolume << "," 
				<< instField->MinLimitOrderVolume << "," 
				<< instField->VolumeMultiple << "," 
				<< instField->PriceTick << "," 
				<< instField->CreateDate << "," 
				<< instField->OpenDate << "," 
				<< instField->ExpireDate << "," 
				<< instField->StartDelivDate << "," 
				<< instField->EndDelivDate << "," 
				<< instField->InstLifePhase << "," 
				<< instField->IsTrading << "," 
				<< instField->PositionType << "," 
				<< instField->PositionDateType << "," 
				<< instField->LongMarginRatio << "," 
				<< instField->ShortMarginRatio << "," 
				<< instField->MaxMarginSideAlgorithm << "," 
				<< instField->UnderlyingInstrID << "," 
				<< instField->StrikePrice << "," 
				<< instField->OptionsType << "," 
				<< instField->UnderlyingMultiple << "," 
				<< instField->CombinationType 
				<< endl;
	}else{
		// title
		output.open("output/instrumentListField.csv");
		output << "��Լ����" << "," 
			<< "����������" << ","
			<< "��Լ����" << "," 
			<< "��Լ�ڽ���������" << "," 
			<< "��Ʒ����" << "," 
			<< "��Ʒ����" << "," 
			<< "�������" << "," 
			<< "������" << ","
			<< "�м۵�����µ���" << "," 
			<< "�м۵���С�µ���" <<","
			<< "�޼۵�����µ���" << "," 
			<< "�޼۵���С�µ���" << "," 
			<< "��Լ��������" <<","
			<< "��С�䶯��λ"  << ","
			<< "������"  << ","
			<< "������"  << ","
			<< "������"  << ","
			<< "��ʼ������"  << ","
			<< "��Լ��������״̬"  << ","
			<< "��ǰ�Ƿ���"  << ","
			<< "�ֲ�����"  << ","
			<< "�ֲ���������"  << ","
			<< "��ͷ��֤����"  << ","
			<< "��ͷ��֤����"  << ","
			<< "�Ƿ�ʹ�ô��߱�֤���㷨"  << ","
			<< "������Ʒ����"  << ","
			<< "ִ�м�"  << ","
			<< "��Ȩ����"  << ","
			<< "��Լ������Ʒ����"  << ","
			<< "�������"  
			<< endl;
		
	}
	output.close();

}

void cTraderSpi::showPositionDetail(){
	this->m_positionCollection->PrintDetail();
}

void cTraderSpi::ReqQryInstrument()
{


	CThostFtdcQryInstrumentField req;
	memset( &req, 0, sizeof(req) );

	while( true )
	{
		int iResult = m_pUserTraderApi->ReqQryInstrument( &req, ++iRequestID );
		char message[256];
		if( !IsFlowControl( iResult ) )
		{
			if( m_genLog )
			{
				sprintf( message, "%s:called cTraderSpi::ReqQryInstrument: %s.", cSystem::GetCurrentTimeBuffer().c_str(), ( ( iResult == 0 ) ? "Success" : "Fail" ) );
				cout << message << endl;
				cSystem::WriteLogFile( m_logFile.c_str(), message, false );
			}
			break;
		}
		else
		{
			if( m_genLog )
			{
				sprintf( message, "%s:called cTraderSpi::ReqQryInstrument: Flow Control.", cSystem::GetCurrentTimeBuffer().c_str() );
				cout << message << endl;
				cSystem::WriteLogFile( m_logFile.c_str(), message, false );
			}
			cSystem::Sleep(1000);
		}
	} // while
}

//void cTraderSpi::ReqQryInvestorPositionDetail()
//{
//	CThostFtdcQryInvestorPositionDetailField qry;
//	memset( &qry, 0, sizeof( qry ) );
//	strcpy( qry.BrokerID, m_brokerID );
//	strcpy( qry.InvestorID, m_investorID );
//
//	while (true)
//	{
//		char message[256];
//		int iResult = m_pUserTraderApi->ReqQryInvestorPositionDetail( &qry, ++iRequestID );
//
//		if( !IsFlowControl( iResult ) )
//		{
//			if( m_genLog )
//			{
//				sprintf( message, "%s:called cTraderSpi::ReqQryInvestorPositionDetail: %s.", cSystem::GetCurrentTimeBuffer().c_str(), ( ( iResult == 0 ) ? "Success" : "Fail" ) );
//				cout << message << endl;
//				cSystem::WriteLogFile( m_logFile.c_str(), message, false );
//			}
//			break;
//		}
//		else
//		{
//			if( m_genLog )
//			{
//				sprintf( message, "%s:called cTraderSpi::ReqQryInvestorPositionDetail: Flow Control.", cSystem::GetCurrentTimeBuffer().c_str() );
//				cout << message << endl;
//				cSystem::WriteLogFile( m_logFile.c_str(), message, false );
//			}
//			cSystem::Sleep(1000);
//		}
//	} 
//}

//void cTraderSpi::OnRspQryInvestorPositionDetail( CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
//{
//	
//	if( m_genLog )
//	{
//		char message[256];
//		sprintf( message, "%s:called cTraderSpi::OnRspQryInvestorPositionDetail.", cSystem::GetCurrentTimeBuffer().c_str() );
//		cout << message << endl;
//		cSystem::WriteLogFile( m_logFile.c_str(), message, false );
//	}
//
//	if( !IsErrorRspInfo( pRspInfo ) && pInvestorPositionDetail )
//	{
//		if( pInvestorPositionDetail->Volume == 0 && !bIsLast )
//			return;
//		m_positionCollection->Add( pInvestorPositionDetail );
//	}
//	else
//	{
//		if( !pInvestorPositionDetail )
//			cout<< "----->> No Position-----"<<endl;
//	}
//	if( bIsLast && !IsErrorRspInfo(pRspInfo) )
//	{
//		//if( _DEBUG && m_positionCollection->Count() > 0 )
//		//{
//		//	printf("\nPosition Detail:\n");
//		//	m_positionCollection->PrintDetail();
//		//}
//		
//		if( m_positionCollection->Count() > 0 )
//		{
//			printf("\nPosition Summary:\n");
//			m_positionCollection->SummaryByInstrument();
//		}
//	}
//	//
//	/* request query of existing orders */
//	//ReqQryOrder();
//}
//
//void cTraderSpi::ReqQryOrder()
//{
//	CThostFtdcQryOrderField req;
//	memset( &req, 0, sizeof( CThostFtdcQryOrderField ) );
//	strcpy( req.BrokerID, m_brokerID );
//	strcpy( req.InvestorID, m_investorID );
//	while( true )
//	{
//		char message[256];
//		int iResult = m_pUserTraderApi->ReqQryOrder( &req, ++iRequestID );
//		if( !IsFlowControl( iResult ) )
//		{
//			sprintf( message, "%s:called cTraderSpi::ReqQryOrder: %s.", cSystem::GetCurrentTimeBuffer().c_str(), ( ( iResult == 0 ) ? "Success" : "Fail") );
//			cout << message << endl;
//			if( m_genLog )
//			{	
//				cSystem::WriteLogFile( m_logFile.c_str(), message, false );
//			}
//			break;
//		}
//		else
//		{
//			sprintf( message, "%s:called cTraderSpi::ReqQryOrder: Flow Control.", cSystem::GetCurrentTimeBuffer().c_str() );
//			cout << message << endl;
//			if( m_genLog )
//			{	
//				cSystem::WriteLogFile( m_logFile.c_str(), message, false );
//			}
//			// sleep 1 sec and re-query existing orders
//			cSystem::Sleep(1000);
//		}
//	} // while
//}

//void cTraderSpi::OnRspQryOrder( CThostFtdcOrderField* pOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast )
//{
//	if( !IsErrorRspInfo( pRspInfo ) )
//		m_orderCollection->Add( pOrder );
//		
//	if( bIsLast && !IsErrorRspInfo(pRspInfo) )
//	{
//	/*	if( m_orderCollection->Count() > 0 )
//			m_orderCollection->PrintAllOrders();*/
//	}
//
//}
void cTraderSpi::ReqOrderInsert(TThostFtdcInstrumentIDType instId,
	TThostFtdcDirectionType dir, TThostFtdcCombOffsetFlagType kpp,
	TThostFtdcPriceType price,   TThostFtdcVolumeType vol)
{
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));	
	strcpy(req.BrokerID, m_brokerID);  
	strcpy(req.InvestorID, m_investorID); 	
	strcpy(req.InstrumentID, instId); 
	strcpy(req.OrderRef, m_ORDER_REF);  
	//��������
	int nextOrderRef = atoi(m_ORDER_REF);
	sprintf(m_ORDER_REF, "%d", ++nextOrderRef);

	req.LimitPrice = price;	//�۸�
	if(0==req.LimitPrice){
		req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;//�۸�����=�м�
		req.TimeCondition = THOST_FTDC_TC_IOC;//��Ч������:������ɣ�������
	}else{
		req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;//�۸�����=�޼�	
		req.TimeCondition = THOST_FTDC_TC_GFD;  //��Ч������:������Ч
	}

	req.Direction = this->MapDirection(dir,true);  //��������	
	req.CombOffsetFlag[0] = this->MapOffset(kpp[0],true); //��Ͽ�ƽ��־:����

	//����Ҳ����
	/*
	req.Direction = dir;
	req.CombOffsetFlag[0] = kpp[0];
	*/

	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	  //���Ͷ���ױ���־	
	req.VolumeTotalOriginal = vol;	///����		
	req.VolumeCondition = THOST_FTDC_VC_AV; //�ɽ�������:�κ�����
	req.MinVolume = 1;	//��С�ɽ���:1	
	req.ContingentCondition = THOST_FTDC_CC_Immediately;  //��������:����

	//TThostFtdcPriceType	StopPrice;  //ֹ���
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;	//ǿƽԭ��:��ǿƽ	
	req.IsAutoSuspend = 0;  //�Զ������־:��	
	req.UserForceClose = 0;   //�û�ǿ����־:��
	if(m_pUserTraderApi){
		int iResult = m_pUserTraderApi->ReqOrderInsert(&req, ++iRequestID);
		if(iResult !=0){
			char message[256];
			sprintf( message, "%s:called cTraderSpi::ReqQryInstrument: %s.", cSystem::GetCurrentTimeBuffer().c_str(), ( ( iResult == 0 ) ? "Success" : "Fail" ) );
			cerr << message << endl;
		}
	}

}

void cTraderSpi::ReqOrderInsert( cOrder* pOrder )
{
	CThostFtdcInputOrderField req;

	memset( &req, 0, sizeof( req ) );
	
	strcpy( req.BrokerID, m_brokerID );
	strcpy( req.InvestorID, m_investorID );
	strcpy( req.UserID, m_investorID );
	strcpy( req.InstrumentID, pOrder->GetInstrumentID().c_str() );
	strcpy( req.OrderRef, m_ORDER_REF );

	req.Direction = pOrder->GetDirection();

	req.CombOffsetFlag[0] = pOrder->GetOffsetFlag();

	req.VolumeTotalOriginal = pOrder->GetVolumeOriginal();

	// order with market price
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;  
	req.LimitPrice = pOrder->GetPrice();  
	req.TimeCondition = THOST_FTDC_TC_GFD;
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	req.VolumeCondition = THOST_FTDC_VC_AV;
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	req.MinVolume = 1;
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	req.IsAutoSuspend = 0;
	req.UserForceClose = 0;

	int iResult = m_pUserTraderApi->ReqOrderInsert( &req, ++iRequestID );
	//cout << "--->>> ReqOrderInsert: " << iResult << ( ( iResult == 0 ) ? ", succeed" : ", fail") << endl;
	if( iResult == 0 )
	{
		int orderRef = atoi( m_ORDER_REF );
		m_allOrderRef.push_back( orderRef );
		sprintf( m_ORDER_REF, "%d", ++orderRef );
		pOrder = NULL;
	}
}

// order insertion response
void cTraderSpi::OnRspOrderInsert( CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast )
{
	//output the order insertion result
	//it seems that this function is called only when error occurs during inserting orders
	
	if( m_genLog )
	{
		printf( "ErrorCode = [%d], ErrorMsg = [%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg );
		char message[256];
		sprintf( message, "%s:called cTraderSpi::OnRspOrderInsert.", cSystem::GetCurrentTimeBuffer().c_str() );
		cout << message << endl;
		cSystem::WriteLogFile( m_logFile.c_str(), message, false );
	}

	if( !IsErrorRspInfo( pRspInfo ) )
	{
	}
}

// order insertion return
void cTraderSpi::OnRtnOrder( CThostFtdcOrderField* pOrder )
{
	if( !IsMyOrder( pOrder ) )
		return;

	if( m_genLog )
	{
		char message[256];
		sprintf( message, "%s:called cTraderSpi::OnRtnOrder.", cSystem::GetCurrentTimeBuffer().c_str() );
		cout << message << endl;
		cSystem::WriteLogFile( m_logFile.c_str(), message, false );
	}
	
	m_orderCollection->Add( pOrder );

	/*if( _DEBUG )
	{
		m_orderCollection->PrintCancelledOrders();
		m_orderCollection->PrintPendingOrders();
	}*/
}



void cTraderSpi::OnRspOrderAction( CThostFtdcInputOrderActionField* pInputOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast )
{
	/* calling this function in case the order cancellation is failed */
	if( !IsErrorRspInfo( pRspInfo ) )
	{
		cout << "OnRspOrderAction" << endl;
	}


}


// transaction notification
// update m_positions
void cTraderSpi::OnRtnTrade( CThostFtdcTradeField* pTrade )
{
	////
	///* update of m_tradeCollection */
	//m_tradeCollection->Add( pTrade );
	//int tradeID = atoi( pTrade->TradeID );
	//if( _DEBUG )
	//	m_tradeCollection->PrintTrade( tradeID );

	////
	///* update of m_positionDetail */
	//if( pTrade->OffsetFlag == '0' )
	//	m_positionCollection->Add( pTrade );
	//else
	//	m_positionCollection->Remove( pTrade );

	//if( _DEBUG )
	//{
	//	if( m_positionCollection->Count() > 0 )
	//	{
	//		printf("\nposition summary:\n");
	//		m_positionCollection->SummaryByInstrument();
	//	}
	//}
}



//void cTraderSpi::ReqQryInvestorPosition()
//{
//	yr_error( "ReqQryInvestorPosition not implemented yet!" );
//}


// initialising trading system..............
//void cTraderSpi::OnRspQryInvestorPosition( CThostFtdcInvestorPositionField* pInvestorPosition, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast )
//{
//	yr_error( "ReqQryInvestorPosition not implemented yet!" );
//}


// the error notification caused by client request
void cTraderSpi::OnRspError(CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast )
{
	printf( "OnRspError:\n" );
	printf( "ErrorCode = [%d], ErrorMsg = [%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg );
	printf( "RequestID = [%d], Chain = [%d]\n", nRequestID, bIsLast );

	// the client should handle the error
}

void cTraderSpi::OnHeartBeatWarning( int nTimeLapse )
{
	cout << "--->>> " << "OnHeartBeatWarning" << endl;
	cout << "--->>> nTimerLapse = " << nTimeLapse << endl;
}


// ReqOrderAction is used for order cancellation
void cTraderSpi::ReqOrderAction( cOrder* pOrder )
{
	CThostFtdcInputOrderActionField req;
	memset( &req, 0, sizeof( req ) );
	strcpy( req.BrokerID, m_brokerID );
	strcpy( req.InvestorID, m_investorID );
	sprintf( req.OrderRef, "%d",pOrder->GetOrderRef() );
	req.FrontID = m_FRONT_ID;
	req.SessionID = m_SESSION_ID;
	req.ActionFlag = THOST_FTDC_AF_Delete;
	strcpy( req.InstrumentID, pOrder->GetInstrumentID().c_str() );

	int iResult = m_pUserTraderApi->ReqOrderAction( &req, ++iRequestID );

}



bool cTraderSpi::IsErrorRspInfo( CThostFtdcRspInfoField* pRspInfo )
{
	bool bResult = ( ( pRspInfo ) && ( pRspInfo->ErrorID != 0 ) );
	if (bResult)
		printf( "ErrorCode = [%d], ErrorMsg = [%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg );
	return bResult;
}

bool cTraderSpi::IsMyOrder( CThostFtdcOrderField* pOrder )
{
	bool flag = false;

	int orderRef = atoi( pOrder->OrderRef );

	for( int i = 0; i < m_allOrderRef.getSize(); ++i )
	{
		if( orderRef == m_allOrderRef[i] )
		{
			flag = true;
			break;
		}
	}

	return( flag && ( pOrder->FrontID == m_FRONT_ID ) && ( pOrder->SessionID == m_SESSION_ID ) );
}


void cTraderSpi::GetInstrumentIDs( cArray< cString >& instrumentIDs ) const
{
	for( int i = 0; i < m_instrumentIDs.getSize(); ++i )
		instrumentIDs.push_back( m_instrumentIDs[i] );
}

const sInstrumentInfo* cTraderSpi::GetInstrumentInfo( const cString& instrumentID ) const
{
	map< cString, sInstrumentInfo* >::const_iterator it = m_instrumentInfo.find( instrumentID );
	if( it != m_instrumentInfo.end() )
		return (*it).second;
	else
		yr_error( "instrument %s not found!", instrumentID.c_str() );
}


void cTraderSpi::RegisterPositionCollection( cPositionCollectionPtr p ) 
{
	if( m_positionCollection.get() )
		m_positionCollection.reset();

	m_positionCollection = p; 
}

void cTraderSpi::RegisterOrderCollection( cOrderCollectionPtr p ) 
{ 
	if( m_orderCollection.get() )
		m_orderCollection.reset();

	m_orderCollection = p; 
}

void cTraderSpi::RegisterTradeCollection( cTradeCollectionPtr p )
{ 
	if( m_tradeCollection.get() )
		m_tradeCollection.reset();

	m_tradeCollection = p; 
}

void cTraderSpi::RegisterInstMessageMap( map<string, CThostFtdcInstrumentField*>* p )
{ 
	if( m_InstMeassageMap )
		m_InstMeassageMap = NULL;

	m_InstMeassageMap = p; 
}

void cTraderSpi::insertOrder(string inst,DIRECTION dire,OFFSETFLAG flag, int vol,double orderPrice){
	TThostFtdcInstrumentIDType    instId;
	TThostFtdcDirectionType       dir;//����,'0'��'1'��
	TThostFtdcCombOffsetFlagType  kpp;//��ƽ��"0"����"1"ƽ,"3"ƽ��
	TThostFtdcPriceType           price;//�۸�0���м�,��������֧��
	strcpy(instId,inst.c_str());

	//double miniChangeTick = m_instMessage_map[inst.c_str()]->PriceTick * 3; // ������ ��С�䶯�۸� ��֤�ɽ�
	double BuyPrice, SellPrice;// ������ �����
	
	BuyPrice = orderPrice;
	SellPrice = orderPrice;

	int longHold,shortHold;
	
	//����
	if(flag == OFFSETFLAG::open){
		//���뿪��
		if( dire == DIRECTION::buy){
			dir = '0';
			strcpy_s(kpp, "0");
			price = BuyPrice;
			this->ReqOrderInsert(instId, dir, kpp, price, vol);
		}
		// ��������
		if( dire == DIRECTION::sell){
			dir = '1';
			strcpy_s(kpp, "0");
			price = SellPrice;
			this->ReqOrderInsert(instId, dir, kpp, price, vol);
			
		}
	}
	if(flag == OFFSETFLAG::close){
	
		//����ƽ��
		if(dire==DIRECTION::buy){
			dir = '0';
			price = BuyPrice;
			this->StraitClose(instId, dir, price, vol);
		}
		//����ƽ��
		if(dire==DIRECTION::sell){
			dir = '1';
			price = SellPrice ;
			this->StraitClose(instId, dir, price, vol);
		}
	}

}
void cTraderSpi::StraitClose(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir,TThostFtdcPriceType price,TThostFtdcVolumeType vol){
	TThostFtdcCombOffsetFlagType  kpp;//��ƽ��"0"����"1"ƽ,"3"ƽ��


	//ƽ�� �� ƽ ���� ƽ�Ķ��
	if(dir == '1'){
		if(this->m_positionCollection->getHolding_long(instId) < vol){
			cerr << "Long:close vol more than hold vol" << endl;
			return;
		}
		if(this->m_positionCollection->getHolding_long(instId)> 0 )
		{
			//������
			if(strcmp(m_InstMeassageMap->at(instId)->ExchangeID, "SHFE") == 0)
			{
				// ��ƽ���ճֲ�
				int Yd_long =this->m_positionCollection->getYdLong(instId);
				int Td_long =this->m_positionCollection->getTdLong(instId);
				if(Td_long > vol)
				{
					//cerr<<"�൥������ƽ��"<<endl;
					strcpy(kpp, "3");//ƽ��
					ReqOrderInsert(instId, dir, kpp, price, vol);
					
				}
				else//��ֲ���ƽ
				{
					//cerr<<"�൥������ͬʱƽ��ƽ�� ��ƽ�� ��ƽ��"<<endl;
					// �н����ƽ��
					if(Td_long>0){
						strcpy(kpp, "3");//ƽ��
						ReqOrderInsert(instId, dir, kpp, price, Td_long);
					}
					strcpy(kpp, "1");//ƽ���
					ReqOrderInsert(instId, dir, kpp, price, vol-Td_long);
				}

			}
			//��������
			else
			{
				
				//cerr<<"���������൥ƽ��:"<<endl;
				strcpy(kpp, "1");
				ReqOrderInsert(instId, dir, kpp, price, vol);
				
			}


		}
	}
	//ƽ��
	else{
		if(this->m_positionCollection->getHolding_short(instId)< vol){
			cerr << "short:close vol more than hold vol" << endl;
			return ;
		}
		
		if(this->m_positionCollection->getHolding_short(instId) > 0)
		{
			//������
			if(strcmp(m_InstMeassageMap->at(instId)->ExchangeID, "SHFE") == 0)
			{
				// ��ƽ���ճֲ�
				int Yd_short = this->m_positionCollection->getYdShort(instId);
				int Td_short = this->m_positionCollection->getTdShort(instId);

				if(Td_short >= vol)
				{
					//cerr<<"�յ�������ƽ��"<<endl;
					strcpy(kpp, "3");//ƽ��
					ReqOrderInsert(instId, dir, kpp, price, vol);
					
				}
				else//û�н��
				{
					//cerr<<"�յ�������ͬʱƽ��ƽ�� ��ƽ�� ��ƽ��"<<endl;
					if(Td_short>0){
						strcpy(kpp, "3");//ƽ��
						ReqOrderInsert(instId, dir, kpp, price, Td_short);
					}
					strcpy(kpp, "1");//ƽ���
					ReqOrderInsert(instId, dir, kpp, price, vol-Td_short);
				}

			}
			//��������
			else
			{
				//cerr<<"���������յ�ƽ��:"<<endl;
				strcpy(kpp, "1");
				ReqOrderInsert(instId, dir, kpp, price, vol);
				
			}



		}
	
	}

}
char cTraderSpi::MapDirection(char src, bool toOrig=true){
	if(toOrig){
		if('b'==src||'B'==src){src='0';}else if('s'==src||'S'==src){src='1';}
	}else{
		if('0'==src){src='B';}else if('1'==src){src='S';}
	}
	return src;
}


char cTraderSpi::MapOffset(char src, bool toOrig=true){
	if(toOrig){
		if('o'==src||'O'==src){src='0';}
		else if('c'==src||'C'==src){src='1';}
		else if('j'==src||'J'==src){src='3';}
	}else{
		if('0'==src){src='O';}
		else if('1'==src){src='C';}
		else if('3'==src){src='J';}
	}
	return src;
}

