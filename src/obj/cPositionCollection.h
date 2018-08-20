#ifndef __CPOSITIONCOLLECTION_H__
#define __CPOSITIONCOLLECTION_H__


#include <map>
#include <list>
#include <string>

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
    int                    getPosition(string instID, DIRE dire);
    int                    getPosition(string instID);// dire undefine,return net position
    int                    getYdPosition(string instID, DIRE dire);
    int                    getTdPosition(string instID, DIRE dire);
    bool                   posDireEqual(DIRE, TThostFtdcPosiDirectionType);
    void                   registerInstFiledMap(cInstrumentFieldMapPtr p);
    std::list<std::string> getTradeButNotPositionInstList();

protected:

    std::multimap<string, cPositionDetailPtr> position_map_;
    cInstrumentFieldMapPtr inst_field_map_; // record instument field

private:
};

typedef shared_ptr<cPositionCollection> cPositionCollectionPtr;

#endif
