#ifndef __CPOSITIONCOLLECTION_H__
#define __CPOSITIONCOLLECTION_H__

#include <map>
#include <list>
#include <string>

#include "cPosition.h"
#include "global.h"
// ��Ҫ����ʵʱ�ĳֲ������㣬�ֲֵ�ӯ�����㣬��Ϊ�漰���ֲֵ�ӯ�� ������ ��֤�� ���ּ۸�ȵ� �˻�������ϢҲ�����ﴦ��
class cPositionCollection {
public:
    cPositionCollection();
    ~cPositionCollection();
    void PrintDetail();
    // update position
    void update(CThostFtdcInvestorPositionField* pInvestorPositionDetail);
    void update(CThostFtdcTradeField* pTrade, shared_ptr<cTrade> pcTrade);
    void update(CThostFtdcDepthMarketDataField* pDepthMarket);
    int  getPosition(string instID, DIRE dire);
    // dire undefine,return net position
    int getPosition(string instID);
    int getYdPosition(string instID, DIRE dire);
    int getTdPosition(string instID, DIRE dire);
    // hold position pnl
    double getPositionPnl(string instId, PNL_TAG pnl_tag,DIRE dire = DIRE::AUTO_UNDEFINE);
    // hold position price
    double getPositionPrc(string instId, PRC_TAG prc_tag,DIRE dire);

        bool               posDireEqual(DIRE, TThostFtdcPosiDirectionType);
    void                   registerInstFiledMap(cInstrumentFieldMapPtr p);
    std::list<std::string> getTradeButNotPositionInstList();

protected:
    std::multimap<string, cPositionDetailPtr> position_map_;
    cInstrumentFieldMapPtr                    inst_field_map_;  // record instument field
};

typedef shared_ptr<cPositionCollection> cPositionCollectionPtr;

#endif
