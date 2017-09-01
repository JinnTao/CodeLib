#include <cPosition.h>
#include <cTrade.h>

cPositionDetail::cPositionDetail(string inst)
{
	memset(this,0,sizeof(this));

	m_lastPrice = 0;//���¼ۣ�ʱ�̱����Լ�����¼ۣ�ƽ����
	m_PreSettlementPrice = 0;//�ϴν���ۣ��Ը�ҹ����ʱ��Ҫ�ã���������
	m_holding_long = 0;//�൥�ֲ���
	m_holding_short = 0;//�յ��ֲ���

	m_TodayPosition_long = 0;//�൥���ճֲ�
	m_YdPosition_long = 0;//�൥���ճֲ�

	m_TodayPosition_short = 0;//�յ����ճֲ�
	m_YdPosition_short = 0;//�յ����ճֲ�

	closeProfit_long = 0;//�൥ƽ��ӯ��
	OpenProfit_long = 0;//�൥����ӯ��

	closeProfit_short = 0;//�յ�ƽ��ӯ��
	OpenProfit_short = 0;//�յ�����ӯ��

	margin = 0;// �ֲ�ռ�ñ�֤��
	this->m_instrumentID = inst;

}



void cPositionDetail::update( CThostFtdcInvestorPositionField* pInvestorPosition )
{

	if(strcmp(pInvestorPosition->InstrumentID,this->m_instrumentID.c_str()) == 0){
		if(pInvestorPosition->PosiDirection == '2')//�൥
		{
			//�൥�ֲ���
			this->m_holding_long += pInvestorPosition->Position;

			//�൥���
			this->m_TodayPosition_long = pInvestorPosition->TodayPosition;

			//�൥��� = �൥�ֲ��� - �൥���
			this->m_YdPosition_long = this->m_holding_long - this->m_TodayPosition_long;//Ҳ����
			//m_trade_message_map[pInvestorPosition->InstrumentID]->YdPosition_long = pInvestorPosition->Position - pInvestorPosition->TodayPosition;

			//�൥ƽ��ӯ��
			this->closeProfit_long =  pInvestorPosition->CloseProfit;

			//�൥����ӯ��(��ʵ�ǳֲ�ӯ������������)
			this->OpenProfit_long = pInvestorPosition->PositionProfit;

			this->margin = pInvestorPosition->UseMargin;

		}
		else if(pInvestorPosition->PosiDirection == '3')//�յ�
		{
			//�յ��ֲ���
			this->m_holding_short += pInvestorPosition->Position;

			//�յ����
			this->m_TodayPosition_short = pInvestorPosition->TodayPosition;

			//�յ���� = �յ��ֲ��� - �յ����
			this->m_YdPosition_short = this->m_holding_short - this->m_TodayPosition_short;

			//�յ�ƽ��ӯ��
			this->closeProfit_short = pInvestorPosition->CloseProfit;

			//�յ��ֲ�ӯ��
			this->OpenProfit_short = pInvestorPosition->PositionProfit;

			this->margin = pInvestorPosition->UseMargin;
		}
	}else{
		yr_error("cPositionDetail update error");
	}
}


void cPositionDetail::Print()
{
	/*printf( " Instrument:%s", m_instrumentID.c_str() );
	printf( " B/S:%s", m_direction == '0' ? "B" : "S" );
	printf( " Volume:%d", m_volume );
	printf( " OpenPrice:%5.3f", m_price );
	printf( " OpenDate:%s", m_openDate.DateToString().c_str() );
	printf( " Type:%s", m_isToday ? "PT" : "PY" );
	printf( " TradeID:%d", m_tradeID );
	printf( "\n" );
*/
	cerr<<this->m_instrumentID
	<<"\t L:"<<this->m_holding_long
	<<"\t S:"<<this->m_holding_short
	<<"\t tL:"<<this->m_TodayPosition_long
	<<"\t yL:"<<this->m_YdPosition_long
	<<"\t tS:"<<this->m_TodayPosition_short
	<<"\t yS:"<<this->m_YdPosition_short
	<<"\t L Close Profit:"<<this->closeProfit_long
	<<"\t L Hold  Profit:"<<this->OpenProfit_long
	<<"\t S Close Profit:"<<this->closeProfit_short
	<<"\t S Hold  Profit:"<<this->OpenProfit_short
	<<"\t margin:"<<this->margin
	<<endl;
}