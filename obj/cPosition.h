#ifndef __CPOSITION_H__
#define __CPOSITION_H__

#include <cString.h>
#include <cDate.h>
#include <stl.h>
#include <ThostFtdcUserApiStruct.h>

class cTrade;

class cPositionDetail
{
public:
	cPositionDetail(string inst);
	void update( CThostFtdcInvestorPositionField* pInvestorPositionDetail );

	~cPositionDetail() {}
	//
	/* Get Method */
	int GetTradeID() const { return m_tradeID; }
	//cString GetInstrumentID() const { return m_instrumentID; }
	//cString GetAccountID() const { return m_accountID; }
	//double GetPrice() const { return m_price; }
	//int GetVolume() const { return m_volume; }
	//cDate GetOpenDate() const { return m_openDate; }
	//bool IsToday() const { return m_isToday; }
	//char GetDirection() const { return m_direction; }
	int getHoldLong(){return this->m_holding_long;}
	int getHoldShort(){return this->m_holding_short;}
	int getYdLong(){return this->m_YdPosition_long;}
	int getTdLong(){return this->m_TodayPosition_long;}

	int getYdShort(){return this->m_YdPosition_short;}
	int getTdShort(){return this->m_TodayPosition_short;}
	//
	/* Set Method */
	//void SetVolume( int volume ) { m_volume = volume; }

	//
	void Print() ;
	
	double closeProfit_long;//�൥ƽ��ӯ��
	
	double OpenProfit_long;//�൥����ӯ��

	double closeProfit_short;//�յ�ƽ��ӯ��
	
	double OpenProfit_short;//�յ�����ӯ��

	double margin;//�ֲ�ռ�ñ�֤��

private:
	int m_tradeID;

	cDate m_openDate;

	string m_instrumentID;//��Լ����

	double m_lastPrice;//���¼ۣ�ʱ�̱����Լ�����¼ۣ�ƽ����

	double m_PreSettlementPrice;//�ϴν���ۣ��Ը�ҹ����ʱ��Ҫ�ã���������
	
	int m_holding_long;//�൥�ֲ���
	
	int m_holding_short;//�յ��ֲ���

	int m_TodayPosition_long;//�൥���ճֲ�
	
	int m_YdPosition_long;//�൥���ճֲ�

	int m_TodayPosition_short;//�յ����ճֲ�
	
	int m_YdPosition_short;//�յ����ճֲ�

};

typedef shared_ptr< cPositionDetail > cPositionDetailPtr;


#endif
