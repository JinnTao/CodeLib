#ifndef __AUTOTRADE_CONFIG_H__
#define __AUTOTRADE_CONFIG_H__

#include <cString.h>
#include <cArray.h>
#include <map>
#include <cSystem.h>
#include <ThostFtdcMdApi.h>
//
struct sATGeneralConfig
{
	bool genLog;
	cString dataoutputDirectory;
	cString tickDataFolderName;
	cString candleDataFolderName;
	cString logFileFolderName;
	cArray< cString > underlyings;
	map< cString, bool > displayTick;
	map< cString, bool > displayCandle;
	map< cString, bool > displaySignal;
	map< cString, bool > displayOrder;
	map< cString, bool > displayTrade;
};

void autotrade_loadconfig_general( sATGeneralConfig& configs, cString& fileName );

//
struct sATDownloadMarketDataConfig
{
	sATGeneralConfig generalConfig;
	cString logFileName;
};

void autotrade_loadconfig_downloadmarketdata( sATDownloadMarketDataConfig& configs );


struct sATBacktestConfig
{
	sATGeneralConfig generalConfig;
	
	cString strategyName;
	cArray< cString > strategyConfigFileNames;
	
	cString dateStart;
	cString dateEnd;
	bool oldFormat;
	cString dataLoadDirectory;
};

void autotrade_loadconfig_backtest( sATBacktestConfig& configs );

struct sATTradeConfig
{
	sATGeneralConfig generalConfig;

	cString logFileName;
	
	cArray< cString > strategyConfigFileNames;
	
};


void autotrade_loadconfig_trade( sATTradeConfig& configs );
//�����ȡ����Ϣ�Ľṹ��
struct AccountParam
{
	TThostFtdcBrokerIDType	brokerId;//���͹�˾����
	TThostFtdcInvestorIDType	userId;//�û���
	char	passwd[252];//����

	char mdAddress[50];//�����������ַ
	char tdAddress[50];//���׷�������ַ

	//string m_read_contract;//��Լ����
	AccountParam(){
		memset(this,0,sizeof(AccountParam));
	}
	void reset(){
		memset(this,0,sizeof(AccountParam));
	}
};

//�����ȡ����Ϣ�Ľṹ��
struct mongoSetting
{
	char mongoHost[50];// Host��ַ
	int mongoPort;// �˿�
	bool mongoLogging;// 

	//string m_read_contract;//��Լ����
	mongoSetting(){
		memset(this,0,sizeof(mongoSetting));
	}
	void reset(){
		memset(this,0,sizeof(mongoSetting));
	}
};

int ParseSettingJson(AccountParam&,mongoSetting&);





#endif