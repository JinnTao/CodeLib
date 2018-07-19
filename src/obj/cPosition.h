#ifndef __CPOSITION_H__
#define __CPOSITION_H__
#include <iostream>
#include <memory>
#include <ThostFtdcUserApiStruct.h>

using namespace std;
class cPositionDetail {
public:
    cPositionDetail(string inst);
    void update(CThostFtdcInvestorPositionField* pInvestorPositionDetail);
    void update(CThostFtdcTradeField* pTrade);
    ~cPositionDetail() {}
    //
    /* Get Method */
    int                             GetTradeID() const { return m_tradeID; }
    string                          GetInstrumentID() const { return m_instrumentID; }
    int                             getPosition() { return position_; }
    int                             getTodayPosition() { return today_pos_; }
    int                             getYdPostion() { return yd_pos_; }
    int                             getPosiDire() { return posi_direction_; }
    void                            Print();
    double                          CloseProfit;     //�൥ƽ��ӯ��
    double                          PositionProfit;  //�൥����ӯ��
    double                          Margin;          //�ֲ�ռ�ñ�֤��
private:
    int    m_tradeID            = 0;
    string m_instrumentID;            //��Լ����
    double m_lastPrice          = 0;  //���¼ۣ�ʱ�̱����Լ�����¼ۣ�ƽ����
    double m_PreSettlementPrice = 0;  //�ϴν���ۣ��Ը�ҹ����ʱ��Ҫ�ã���������
    double position_cost_       = 0;  //�ֲֳɱ�
    double open_cost_           = 0;  //���ֳɱ�
    int    position_            = 0;  //�ֲܳ���
    int    today_pos_           = 0;  //���ճֲ���
    int    yd_pos_              = 0;  //�൥���ճֲ�
    int    posi_direction_      = 0;  // 1 : �൥ -1 : �յ� 0 : δ����
    string trade_date_;
};

typedef shared_ptr<cPositionDetail> cPositionDetailPtr;
#endif
