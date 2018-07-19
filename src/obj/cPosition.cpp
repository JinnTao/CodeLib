#include <cPosition.h>
#include <cTrade.h>
#include <easylogging++.h>
cPositionDetail::cPositionDetail(string inst) {
    memset(this, 0, sizeof(this));

    this->m_instrumentID = inst;
}

void cPositionDetail::update(CThostFtdcInvestorPositionField* pInvestorPosition) {

    if (strcmp(pInvestorPosition->InstrumentID, this->m_instrumentID.c_str()) == 0) {
        // long postion
        if (pInvestorPosition->PosiDirection == '2') {
            posi_direction_ = 1;
        }
        // short postion
        else if (pInvestorPosition->PosiDirection == '3') {
            posi_direction_ = -1;
        } else {
            LOG(INFO) << "cPostionDetail PosiDirection error";
            return;
        }
        position_ = pInvestorPosition->Position;
        today_pos_ = pInvestorPosition->TodayPosition;
        yd_pos_ = pInvestorPosition->YdPosition;
        Margin = pInvestorPosition->UseMargin;
        CloseProfit = pInvestorPosition->CloseProfit;
        PositionProfit = pInvestorPosition->PositionProfit;
        trade_date_    = pInvestorPosition->PositionDate;
    } else {
        LOG(INFO) << "cPostionDetail update error";
    }
}
//  identification long & short at outer func.. 
void cPositionDetail::update(CThostFtdcTradeField* pTrade) {

    if (strcmp(pTrade->InstrumentID, this->m_instrumentID.c_str()) == 0) {
        if (pTrade->OffsetFlag == THOST_FTDC_OF_Open) {
            position_ += pTrade->Volume;

            today_pos_ += pTrade->Volume;
        }
        if (pTrade->OffsetFlag == THOST_FTDC_D_Sell) {
            position_ -= pTrade->Volume;
            //��������������ֻ����������Ч
            if (pTrade->OffsetFlag == THOST_FTDC_OF_Close || pTrade->OffsetFlag == THOST_FTDC_OF_CloseYesterday ||
                pTrade->OffsetFlag == THOST_FTDC_OF_ForceClose)
                yd_pos_ -=  pTrade->Volume;  //���
            else if (pTrade->OffsetFlag == THOST_FTDC_OF_CloseToday)
                today_pos_ -=  pTrade->Volume;  //���

            //������5�֣����1��ƽ�ֶ��Ƿ�'1'������ƽ��2�֣����������-1����ֻ���5�֣�ʵ��Ӧ���ǽ��5-1�����0
            // 3����֣�5�ֽ�֣���'1'ƽ����4��,���������-1����ֻ���5�֣�ʵ��Ӧ���ǽ��5-1�����0
            if (this->yd_pos_ < 0) {
                this->today_pos_ += this->yd_pos_;
                this->yd_pos_              = 0;
            }
        }
    } 
    else {
        LOG(INFO) << "cPostionDetail update error";
    }
 }

void cPositionDetail::Print() {
    cerr << this->m_instrumentID 
        << "\t Pos:" << position_ 
        << "\t Td:" << today_pos_
        << "\t Yd:" << yd_pos_
        << "\t PositionP&L:" << PositionProfit
        << "\t CloseP&L:" << CloseProfit
        << "\t margin:" << Margin 
        << "\t tradeDate:" << trade_date_ << endl;
}