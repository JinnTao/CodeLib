#ifndef __AUTOTRADE_CONFIG_H__
#define __AUTOTRADE_CONFIG_H__

#include <cString.h>
#include <cArray.h>
#include <map>
#include <cSystem.h>
#include <ThostFtdcMdApi.h>
#include <memory>
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
	char address[50];// Host��ַ
    char database[50];
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

struct autoSetting
{
	char tradeDayDir[125];// ������·��
	char dataBaseDir[125];// ����·��
	
    
    char startDate[25];
	char endDate[25];

// dataSource
    char startDateTime[150];
    char endDateTime[150];
    char collectionName[20];

	//kingKeltNer
	//;����ͨ����ֵ�Ĵ�����
	double kkLength;// = 11
	//;����ͨ����ȵ�ƫ��
	double kkDev;// = 1.6
	//;�ƶ�ֹ��
	double trailingPrcnt;// = 0.8
	//;ÿ�ν��׵�����
	double fixedSize;// = 1
	//;��ʼ���������õ�����
	double initDays;// = 10

    // strategy base Information
    char inst[300]; // strategy Trading Inst
    char lots[300]; // strategy Trading Inst lost
    char timeMode[300]; // strategy Time filter mode
    char collectionList[500]; // strategy collection name

	autoSetting(){
		memset(this, 0, sizeof(autoSetting));
	}
	void reset(){
		memset(this, 0, sizeof(autoSetting));
	}
};

int ParseSettingJson(AccountParam&,mongoSetting&,autoSetting&);





#endif