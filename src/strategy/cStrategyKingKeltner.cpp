#include "cStrategyKingKeltner.h"

cStrategyKingKeltner::cStrategyKingKeltner(void) : cStrategy() {
    name_ = "KingKeltner";
}
cStrategyKingKeltner::~cStrategyKingKeltner(void){}

void cStrategyKingKeltner::onInit(){
    std::vector<std::string> inst_list;
    inst_list.push_back("rb1901");
    subcribe(inst_list, 60, 100, STRATEGY_MODE::REAL);
}
void cStrategyKingKeltner::onLoop(contextPtr context_ptr){
    // =================================================================  ָ�����
    double up, down;
    if (!keltner(int(this->m_pAutoSetting->kkLength), this->m_pAutoSetting->kkDev, up, down)) {
        return;
    }
    //=============================================================ȡ��ǰ������δ�ɽ���
    this->m_pTradeSpi->cancleMyPendingOrder();
    this->m_workingStopOrderList.clear();
    // ===========================================================�µ��߼�============================================================

    int longPos  = this->m_pPositionC.get()->getPosition(m_inst, DIRE::AUTO_LONG);
    int shortPos = this->m_pPositionC.get()->getPosition(m_inst, DIRE::AUTO_SHORT);

    this->m_netPos = longPos - shortPos;

    if (m_netPos == 0) {
        this->m_intraTradeHigh = m_lastHigh;
        this->m_intraTradeLow  = m_lastLow;
        // according strategy config set trading lots
        this->sendOcoOrder(up, down, int(m_lots));

    } else if (m_netPos > 0) {
        m_intraTradeHigh = max(m_lastHigh, m_intraTradeHigh);
        m_intraTradeLow  = m_lastLow;
        this->sendStopOrder(m_inst,
                            traderTag::DIRECTION::sell,
                            traderTag::OFFSETFLAG::close,
                            m_intraTradeHigh * (1 - m_pAutoSetting->trailingPrcnt / 100.0),
                            UINT(std::abs(m_netPos)),
                            this->m_strategyName);

    } else if (m_netPos < 0) {
        m_intraTradeHigh = m_lastHigh;
        m_intraTradeLow  = min(m_lastLow, m_intraTradeLow);
        this->sendStopOrder(m_inst,
                            traderTag::DIRECTION::buy,
                            traderTag::OFFSETFLAG::close,
                            m_intraTradeLow * (1 + m_pAutoSetting->trailingPrcnt / 100.0),
                            UINT(std::abs(m_netPos)),
                            this->m_strategyName);
    }
    // ==============================================================��־���========================================================
    // double rsiValue = outReal[0];
    ILOG("NetPos:{},Up:{},Down:{},LastClosePrice:{}.", m_netPos, up, down, m_lastClose);
    printStatus();
}

void cStrategyKingKeltner::sendOcoOrder(std::string inst,double  upPrice, double downPrice, int fixedSize) {
    //
    //    ����OCOί��
    //    OCO(One Cancel Other)ί�У�
    //    1. ��Ҫ����ʵ������ͻ���볡
    //    2. �������������෴��ֹͣ��
    //    3. һ�������ֹͣ���ɽ��������������һ�������
    this->buyOpen(inst, upPrice, fixedSize, true);
    this->sellOpen(inst, downPrice, fixedSize, true);
}

void cStrategyKingKeltner::printStatus() {
    for each (auto var in stop_order_list_)
    {
        if(var.status){
            time_t orderTimeT = std::chrono::system_clock::to_time_t(var.orderTime);

            struct tm* ptm = localtime(&orderTimeT);
            char date[60] = { 0 };
            sprintf(date, "%d-%02d-%02d %02d:%02d:%02d",
                (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
                (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
            string orderDateTime = string(date);
            ILOG("{} {} stop order {} {} {} {} {} {}.",
                 orderDateTime,
                 var.instrument,
                 ((var.direction == traderTag::DIRECTION::buy) ? "buy" : "sell"),
                 ((var.offset == traderTag::OFFSETFLAG::close) ? "close " : "open "),
                 var.price,
                 var.volume,
                 var.slipTickNum,
                 var.strategyName);

        }
    }
}



