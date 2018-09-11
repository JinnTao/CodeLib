#ifndef __CPOSITION_H__
#define __CPOSITION_H__
#include <iostream>
#include <map>

#include "ThostFtdcUserApiStruct.h"
#include "cTrade.h"
using namespace std;

enum DIRE { AUTO_LONG, AUTO_SHORT, AUTO_UNDEFINE };
enum PNL_TAG { CLOSE_PNL, POSI_PNL, FLOAT_PNL };
enum PRC_TAG {OPEN_COST,POSI_COST};
using cInstrumentFieldMapPtr = shared_ptr<std::map<std::string, std::shared_ptr<CThostFtdcInstrumentField>>>;
class cPositionDetail {
public:
    cPositionDetail(string inst);
    void update(CThostFtdcInvestorPositionField* pInvestorPositionDetail);
    void update(CThostFtdcTradeField* pTrade, shared_ptr<cTrade> pcTrade);
    void update(CThostFtdcDepthMarketDataField* pDepthMarketData);
    ~cPositionDetail() {}
    //
    /* Get Method */
    int    GetTradeID() const { return m_tradeID; }
    string GetInstrumentID() const { return instrument_id_; }
    int    getPosition() { return position_; }
    int    getTodayPosition() { return today_pos_; }
    int    getYdPostion() { return yd_pos_; }
    DIRE   getPosiDire() { return posi_direction_; }
    double getCommission() { return commission_; }
    void   registerInstField(std::shared_ptr<CThostFtdcInstrumentField> p) { inst_field_ = p; }
    void   Print();
    double CloseProfit    = 0;  //ƽ��ӯ��
    double PositionProfit = 0;  //�ֲ�ӯ��
    double FloatProfit    = 0;  // ����ӯ�� ���ۼ�ӯ����
    double Margin         = 0;  //�ֲ�ռ�ñ�֤��
private:
    int                                        m_tradeID = 0;
    string                                     instrument_id_;       //��Լ����
    double                                     position_price_ = 0;  //�ֲֳɱ�
    double                                     open_price_     = 0;  //���ֳɱ�
    int                                        position_       = 0;  //�ֲܳ���
    int                                        today_pos_      = 0;  //���ճֲ���
    int                                        yd_pos_         = 0;  //�൥���ճֲ�
    string                                     exchange_id_;         // ������
    double                                     commission_     = 0;
    double                                     settle_price_   = 0;              // �����
    double                                     last_price_     = 0;              // �г��۸�
    double                                     margin_rate_    = 0;              // ��֤����
    double                                     open_cost_      = 0;              // ���ֽ��
    double                                     position_cost_  = 0;              // �ֲֽ��
    DIRE                                       posi_direction_ = AUTO_UNDEFINE;  //
    string                                     trade_date_;
    string                                     position_date_;
    string                                     update_time_;
    std::shared_ptr<CThostFtdcInstrumentField> inst_field_;  // ��Լ��Ϣ
};

typedef shared_ptr<cPositionDetail> cPositionDetailPtr;
#endif
