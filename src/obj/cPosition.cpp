#include <cPosition.h>
#include <cTrade.h>
#include <easylogging++.h>
cPositionDetail::cPositionDetail(string inst) {
    memset(this, 0, sizeof(this));

    this->m_instrumentID = inst;
    position_price_      = 0;  //�ֲֳɱ�
    open_price_          = 0;  //���ֳɱ�
    position_            = 0;  //�ֲܳ���
    today_pos_           = 0;  //���ճֲ���
    yd_pos_              = 0;  //�൥���ճֲ�
    commission_          = 0;
    settle_price_        = 0;  // �����
    last_price_          = 0;  // �г��۸�
    margin_rate_         = 0;  // ��֤����
    CloseProfit          = 0;  //ƽ��ӯ��
    PositionProfit       = 0;  //�ֲ�ӯ��
    FloatProfit          = 0;  // ����ӯ�� ���ۼ�ӯ����
    Margin               = 0;  //�ֲ�ռ�ñ�֤��
}

void cPositionDetail::update(CThostFtdcInvestorPositionField* pInvestorPosition) {
    double lamda = 1;
    if (strcmp(pInvestorPosition->InstrumentID, this->m_instrumentID.c_str()) == 0) {

        Margin         += pInvestorPosition->UseMargin;
        CloseProfit    += pInvestorPosition->CloseProfit;
        PositionProfit += pInvestorPosition->PositionProfit;
        commission_    += pInvestorPosition->Commission;

        //LOG(INFO)<< "Inst" << pInvestorPosition->InstrumentID << "position" << pInvestorPosition->Position 
        //          <<"ydPos" << pInvestorPosition->YdPosition 
        //          << " tdPos " << pInvestorPosition->TodayPosition
        //         << " Margin " << pInvestorPosition->UseMargin
        //         << " CloseProfit " << pInvestorPosition->CloseProfit << " PositionProfit " << pInvestorPosition->PositionProfit
        //          << " position_date_ " << pInvestorPosition->PositionDate << " commission_ "
        //          << pInvestorPosition->Commission << " settle_price_ " << pInvestorPosition->SettlementPrice
        //          << " margin_rate_ " << pInvestorPosition->ExchangeMargin    
        //    << "openVolume"<< pInvestorPosition->OpenVolume 
        //    << " closeV " << pInvestorPosition->CloseVolume;

        // long postion
        if (pInvestorPosition->PosiDirection == THOST_FTDC_PD_Long) {
            posi_direction_ = DIRE::AUTO_LONG;
            lamda           = 1;
        }
        // short postion
        else if (pInvestorPosition->PosiDirection == THOST_FTDC_PD_Short) {
            posi_direction_ = DIRE::AUTO_SHORT;
            lamda           = -1;
        } else {
            LOG(INFO) << "cPostionDetail PosiDirection error";
        }
        if (pInvestorPosition->Position > 0) {
            position_ += pInvestorPosition->Position;
            today_pos_ += pInvestorPosition->TodayPosition;
            yd_pos_ += pInvestorPosition->YdPosition;
            settle_price_   = pInvestorPosition->SettlementPrice;
            margin_rate_    = pInvestorPosition->ExchangeMargin;
            position_date_  = pInvestorPosition->PositionDate;
            trade_date_     = pInvestorPosition->TradingDay;

            open_price_     = pInvestorPosition->OpenCost / double(inst_field_->VolumeMultiple) / double(position_);
            position_price_ = pInvestorPosition->PositionCost / double(inst_field_->VolumeMultiple) / double(position_);
            last_price_ = position_price_ + lamda * PositionProfit / double(position_ * double(inst_field_->VolumeMultiple));
            FloatProfit = lamda * (last_price_ - open_price_) * position_ * double(inst_field_->VolumeMultiple);
        }
    } else {
        LOG(INFO) << "cPostionDetail update error";
    }
}

void cPositionDetail::update(CThostFtdcDepthMarketDataField* pDepthMarketData) {
    if (strcmp(pDepthMarketData->InstrumentID, this->m_instrumentID.c_str()) == 0) {
        last_price_  = pDepthMarketData->LastPrice;
        exchange_id_ = pDepthMarketData->ExchangeID;
        update_time_ = string(pDepthMarketData->TradingDay + string(" ") + pDepthMarketData->UpdateTime);
        if (position_ > 0) {
            if (posi_direction_ == DIRE::AUTO_LONG) {
                PositionProfit = (last_price_ - position_price_) * position_ * double(inst_field_->VolumeMultiple);
                FloatProfit    = (last_price_ - open_price_) * position_ * double(inst_field_->VolumeMultiple);
            }
            if (posi_direction_ == DIRE::AUTO_SHORT) {
                PositionProfit = (position_price_ - last_price_) * position_ * double(inst_field_->VolumeMultiple);
                FloatProfit    = (open_price_ - last_price_) * position_ * double(inst_field_->VolumeMultiple);
            }
        }
    } else {
        LOG(INFO) << "cPositionDetail update marketData inst not match.";
    }
}
//  identification long & short at outer func..
void cPositionDetail::update(CThostFtdcTradeField* pTrade) {

    if (strcmp(pTrade->InstrumentID, this->m_instrumentID.c_str()) == 0) {

        int old_pos = position_;

        // open
        if (pTrade->OffsetFlag == THOST_FTDC_OF_Open) {
            position_ += pTrade->Volume;
            today_pos_ += pTrade->Volume;
            //LOG(INFO) << pTrade->Price << "  " << pTrade->Volume << "  " << open_price_ << "  " << old_pos << "  "
             //         << position_;
            open_price_     = (pTrade->Price * pTrade->Volume + open_price_ * old_pos) / double(position_);
            position_price_ = (pTrade->Price * pTrade->Volume + position_price_ * old_pos) / double(position_);
            // long
            if (pTrade->Direction == THOST_FTDC_D_Buy) {
                posi_direction_ = DIRE::AUTO_LONG;
            }
            // short
            if (pTrade->Direction == THOST_FTDC_D_Sell) {
                posi_direction_ = DIRE::AUTO_SHORT;
            }
        }
        // close
        else {
            position_ -= pTrade->Volume;
            //��������������ֻ����������Ч
            if (pTrade->OffsetFlag == THOST_FTDC_OF_Close || pTrade->OffsetFlag == THOST_FTDC_OF_CloseYesterday ||
                pTrade->OffsetFlag == THOST_FTDC_OF_ForceClose)
                yd_pos_ -= pTrade->Volume;  //���
            else if (pTrade->OffsetFlag == THOST_FTDC_OF_CloseToday)
                today_pos_ -= pTrade->Volume;  //���

            //������5�֣����1��ƽ�ֶ��Ƿ�'1'������ƽ��2�֣����������-1����ֻ���5�֣�ʵ��Ӧ���ǽ��5-1�����0
            // 3����֣�5�ֽ�֣���'1'ƽ����4��,���������-1����ֻ���5�֣�ʵ��Ӧ���ǽ��5-1�����0
            if (this->yd_pos_ < 0) {
                this->today_pos_ += this->yd_pos_;
                this->yd_pos_ = 0;
            }
            // buy close => sell open
            if (pTrade->Direction == THOST_FTDC_D_Buy) {
                CloseProfit += (open_price_ - pTrade->Price) * pTrade->Volume * double(inst_field_->VolumeMultiple);
            }
            // sell close => buy open
            if (pTrade->Direction == THOST_FTDC_D_Sell) {
                CloseProfit += (pTrade->Price - open_price_) * pTrade->Volume * double(inst_field_->VolumeMultiple);
            }
            if (position_ > 0) {
                //open_price_     = (open_price_ * old_pos - pTrade->Price * pTrade->Volume) / double(position_);
                //position_price_ = (position_price_ * old_pos - pTrade->Price * pTrade->Volume) / double(position_);
                
            } else {
                open_price_     = 0;
                position_price_ = 0;
                PositionProfit  = 0;
                FloatProfit     = 0;
            }

        }

        position_date_ = pTrade->TradingDay;
        trade_date_    = pTrade->TradingDay;
        exchange_id_   = pTrade->ExchangeID;
        //LOG(INFO) << "price :" << pTrade->Price << " pos: " << position_ << "tradding day" << trade_date_;

    } else {
        LOG(INFO) << "cPostionDetail update error";
    }
}

void cPositionDetail::Print() {
    string posi_dire[] = {"Long", "Short","error"};
    cerr << this->m_instrumentID << "\t Pos:" << position_ << "\t Dire: " << posi_dire[posi_direction_]
         << "\t Td:" << today_pos_ << "\t Yd:" << yd_pos_ << "\t PositionP&L:" << PositionProfit
         << "\t Float P&L:" << FloatProfit  //<< "\t CloseP&L:" << CloseProfit
         << "\t openPrice:" << open_price_ << "\t posiPrice: " << position_price_ << "\t lastPrice: " << last_price_
         << "\t updateTime:" << update_time_ << endl;
}