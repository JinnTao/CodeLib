#include "cStrategyKingKeltner.h"
cStrategyKingKeltner::cStrategyKingKeltner(void) : cStrategy() {
    name_ = "KingKeltner";
}
void cStrategyKingKeltner::onInit(){
    std::vector<std::string> inst_list;
    inst_list.push_back("rb1901");
    subscribe(inst_list, 60, 100, STRATEGY_MODE::REAL);// 60������Ƶ�� ���ݳ���100�� ģʽ��ʵ��
}
void cStrategyKingKeltner::onLoop(contextPtr context_ptr){
    std::lock_guard<std::mutex> lock(global::run_mutex);
    double      up, down, dev = 1.6, trailingPrcnt = 0.8;
    int         n = 11, lots = 1;
    std::string trade_inst = "rb1901";
    context_ptr->at(trade_inst).keltner(n, dev, up, down);
    int     count    = context_ptr->at(trade_inst).count();
    barData last_bar = context_ptr->at(trade_inst).lastBarData(--count);
    
    cancelAllOrder();// ȡ��ǰ������δ�ɽ���
    
    int netPos  = position_collection_->getPosition(trade_inst);
    if (netPos == 0) {
        intra_high_ = last_bar.high;
        intra_low_  = last_bar.low;
        sendOcoOrder(trade_inst, up, down, lots);
    } else if (netPos > 0) {
        intra_high_     = max(last_bar.high, intra_high_);
        intra_low_  = last_bar.low;
        sellClose(trade_inst, intra_high_ * (1 - trailingPrcnt / 100.0), lots, true);
    } else if (netPos < 0) {
        intra_high_     = last_bar.high;
        intra_low_  = min(last_bar.low, intra_low_);
        buyClose(trade_inst, intra_low_ * (1 + trailingPrcnt / 100.0), lots, true);
    }
    // ��־���
    ILOG("NetPos:{},Up:{},Down:{},LastClosePrice:{}.", lots, up, down, last_bar.close);
    showStopOrders();
}
//   ʵ������ͻ���볡
void cStrategyKingKeltner::sendOcoOrder(std::string inst,double  upPrice, double downPrice, int fixedSize) {
    this->buyOpen(inst, upPrice, fixedSize, true);
    this->sellOpen(inst, downPrice, fixedSize, true);
}
void cStrategyKingKeltner::onOrder(cOrderPtr) {}
void cStrategyKingKeltner::onTrade(CThostFtdcTradeField) {}
cStrategyKingKeltner::~cStrategyKingKeltner(void) {}