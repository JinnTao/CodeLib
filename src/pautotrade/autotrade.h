#ifndef __AUTOTRADE_H__
#define __AUTOTRADE_H__

#include <cMarketDataCollection.h>
#include <cSystem.h>
#include <cMdSpi.h>
#include <cTraderSpi.h>
#include <cThread.h>
#include <cStrategy.h>
#include <cTradingPlatform.h>



//
#include <autotrade_config.h>

typedef cThread< cTradingPlatform > AutoTradeThread;

void autotrade_trade();

#endif
