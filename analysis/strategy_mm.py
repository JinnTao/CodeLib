# -*- coding: utf-8 -*-

__author__ = 'JinnTao'

import tqsdk
import datetime as dt
from contextlib import closing
import time
import numpy as np
import pandas as pd

'''
    做市商策略
    bid/ask = mid + alpha + risk +/- spread
    aplha = 0
    risk = position * price_tick
    price_tick : 最小报价单位
    spread:  2 * trade_fees
    trade_fees : 手续费
'''
TRADE_SYMBOL = "DCE.i1909"
TICK_DATA_LEN = 10
TRADE_FEES = 0.5  # 元/吨
PRICE_TICK = 0.5
STICK_RATIO = 0.90
MAX_POS = 5
MAX_QUOTE_POS = 2
CLOSE_HOUR = 14
CLOSE_MIN = 50

api = tqsdk.TqApi(tqsdk.TqSim(), backtest=tqsdk.TqBacktest(
    start_dt=dt.datetime(2019, 5, 14, 21, 00, 0), end_dt=dt.datetime(2019, 5, 15, 15, 0, 0)))

ticks = api.get_tick_serial(symbol=TRADE_SYMBOL, data_length=TICK_DATA_LEN)

position = api.get_position(TRADE_SYMBOL)

account = api.get_account()

target_pos = tqsdk.TargetPosTask(api, TRADE_SYMBOL)

buy_order = {}
sell_order = {}


def obtain_position_risk():
    pos = position["volume_long"] - position["volume_short"]
    return pos * PRICE_TICK * -1.0


def obtain_symbol_alpha():
    alpha = 0
    return alpha


def apply_sticky_prc(quote, quote_market, spread, dire):
    sticky_pts = STICK_RATIO * abs(spread)
    val = 0
    if abs(quote - quote_market) < sticky_pts:
        val = quote_market
    else:
        val = quote
    if dire > 0:
        val = val + PRICE_TICK
    val = val -  val % PRICE_TICK
    return val


def clear_order():
    api.cancel_order(sell_order)
    api.cancel_order(buy_order)


def near_close_pos_time_point(now):
    if now.hour == CLOSE_HOUR and now.minute >= CLOSE_MIN:  # 到达平仓时间: 平仓
        print("临近本交易日收盘: 平仓")
        target_pos.set_target_volume(0)  # 平仓
        deadline = time.time() + 60
        while api.wait_update(deadline=deadline):  # 等待60秒
            pass
        api.close()
    return

def make_order(dire,volume,limit_prc):
    global  buy_order,sell_order
    if dire == "BUY":
        if position["volume_short"] > 0:
            buy_order = api.insert_order(symbol=TRADE_SYMBOL,
                                         direction="BUY", offset="CLOSETODAY", volume=volume, limit_price=limit_prc)
        else:
            buy_order = api.insert_order(symbol=TRADE_SYMBOL,
                                         direction="BUY", offset="OPEN", volume=volume, limit_price=limit_prc)
    elif dire == "SELL":
        if position["volume_long"] > 0:
            sell_order = api.insert_order(symbol=TRADE_SYMBOL,
                                         direction="SELL", offset="CLOSETODAY", volume=volume, limit_price=limit_prc)
        else:
            sell_order = api.insert_order(symbol=TRADE_SYMBOL,
                                         direction="SELL", offset="OPEN", volume=volume, limit_price=limit_prc)
def update_buy_order(bid_size,bid):
    global  buy_order
    if buy_order :
        # 全部成交
        if buy_order["volume_left"] == 0:
            make_order("BUY",bid_size,bid)
        # 部分成交
        elif abs(buy_order['limit_price'] - bid) > 2 * PRICE_TICK:
            api.cancel_order(buy_order)
            make_order( "BUY", bid_size, bid)

    else:
        make_order("BUY",bid_size,bid)

def update_sell_order(ask_size,ask):
    global  sell_order
    if sell_order:
        # 全部成交
        if sell_order["volume_left"] == 0:
            make_order("SELL", ask_size, ask)
        # 部分成交
        elif abs(sell_order['limit_price'] - ask) > 2 * PRICE_TICK:
            api.cancel_order(sell_order)
            make_order("SELL", ask_size, ask)
    else:
        make_order("SELL", ask_size, ask)


with closing(api):
    while True:
        api.wait_update()
        if api.is_changing(ticks):
            last_tick = ticks.iloc[-1, :]
            mid = (last_tick.ask_price1 * last_tick.ask_volume1 + last_tick.bid_price1 * last_tick.bid_volume1) / \
                  (last_tick.ask_volume1 + last_tick.bid_volume1)

            pos = position["volume_long"] - position["volume_short"]
            core_spread = TRADE_FEES * 2
            bid_spread = 0.5 * core_spread
            ask_spread = 0.5 * core_spread

            alpha = obtain_symbol_alpha()
            risk = obtain_position_risk()

            ask_spread = max(0.0, ask_spread + alpha + risk)
            bid_spread = max(0.0, bid_spread + alpha + risk)

            bid = max(0.0, mid - bid_spread)
            ask = max(0.0, mid + ask_spread)

            bid = apply_sticky_prc(bid, last_tick.bid_price1, bid_spread, -1)
            ask = apply_sticky_prc(ask, last_tick.ask_price1, ask_spread, 1)

            if (bid == ask):
                bid = last_tick.bid_price1
                ask = last_tick.ask_price1
            # one last defense
            # to do

            # size
            if position['volume_long'] > 0 and position['volume_long'] < MAX_QUOTE_POS:
                ask_size = position['volume_long']
            else:
                ask_size = min(MAX_QUOTE_POS, max(0, MAX_POS + pos))
            if position['volume_short'] > 0 and position['volume_short'] < MAX_QUOTE_POS:
                bid_size = position['volume_short']
            else:
                bid_size = min(MAX_QUOTE_POS, max(0, MAX_POS - pos))
            now = dt.datetime.fromtimestamp(int(last_tick.datetime / 1e9))
            #print("Datetime:{},account:{},float_profit:{},close_profit:{}".format(now, account['static_balance'],
            #                                                                      account['float_profit'],
            #                                                                      account['close_profit']))
            print("long:",position["volume_long"], "short:",position["volume_short"])
            if sell_order:
                print(sell_order['direction'], sell_order['offset'], sell_order['volume_orign'],
                      sell_order['volume_left'], sell_order['limit_price'])
            if buy_order:
                print(buy_order['direction'],buy_order['offset'],buy_order['volume_orign'],
                      buy_order['volume_left'],buy_order['limit_price'])


            # print("datetime:{},bid:{},bid_spread:{},bid_size:{},ask:{},ask_spread:{},ask_size:{}".format(
            #     now,
            #     bid, bid_spread, bid_size,ask, ask_spread, ask_size))
            # print("m_bid:{},m_ask:{}".format(
            #     last_tick.bid_price1,last_tick.ask_price1))

            #clear_order()
            if bid_size > 0:
                update_buy_order(bid_size,bid)
            else:
                api.cancel_order(buy_order)
                buy_order = {}
            if ask_size > 0 :
                update_sell_order(ask_size,ask)
            else:
                api.cancel_order(sell_order)
                sell_order = {}


            near_close_pos_time_point(now)

            # risk stop Loss
            if account['float_profit'] < -5000:
                api.cancel_order(buy_order)
                buy_order = {}
                api.cancel_order(sell_order)
                sell_order = {}
