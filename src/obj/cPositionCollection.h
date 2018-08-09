#ifndef __CPOSITIONCOLLECTION_H__
#define __CPOSITIONCOLLECTION_H__


#include <map>
#include <list>

#include "cPosition.h"

class cPositionCollection {
public:
    cPositionCollection();
    virtual ~cPositionCollection();
    void PrintDetail();
    /*update position*/
    void update(CThostFtdcInvestorPositionField* pInvestorPositionDetail);
    void update(CThostFtdcTradeField* pTrade);
    void update(CThostFtdcDepthMarketDataField* pDepthMarket);
    int  getPosition(string instID, DIRE dire);
    int  getYdPosition(string instID, DIRE dire);
    int  getTdPosition(string instID, DIRE dire);
    bool posDireEqual(DIRE, TThostFtdcPosiDirectionType);
    void registerInstFiledMap(cInstrumentFieldMapPtr p){inst_field_map_ = p;};

protected:
    //����򵥵ķ�ʽʵ�֣����̫���tyedef �ǲ��ǽṹ���ڸ��ӣ��������µ��뷨��
    // mapType _map_position;
    // mapType::iterator _it;
    // positionStore _m_position_instrument;
    // positionHandle _m_position_tradeid;
    std::multimap<string, cPositionDetailPtr> position_map_;
    cInstrumentFieldMapPtr inst_field_map_;

private:
};

typedef shared_ptr<cPositionCollection> cPositionCollectionPtr;

#endif
