#pragma once

#include "Order.h"
#include "Usings.h"
#include "Trade.h"
#include "OrderModify.h"
#include "OrderbookLevelInfos.h"

#include <map>
#include <unordered_map>
#include <list>
#include <memory>

class Orderbook
{
private:
    std::map<Price, OrderPtrs, std::greater<Price>> bids_; // best bid (highest price) at begin()
    std::map<Price, OrderPtrs, std::less<Price>> asks_;    // best ask (lowest price) at begin()

    // for O(1) fast lookup in a list. e.g., order cancellation
    struct OrderEntry
    {
        OrderPtr order_;
        OrderPtrs::iterator location_;
    };
    std::unordered_map<OrderId, OrderEntry> orders_;

    struct LevelData
    {
        Quantity quantity_{0};   // e.g., total shares from all orders
        Quantity orderCount_{0}; // number of orders

        enum class Action
        {
            Add,
            Remove,
            Match
        };
    };
    std::unordered_map<Price, LevelData> data_;

public:
    // constructor and destructor
    Orderbook() = default;
    ~Orderbook() = default;

    // getters
    const auto &getBids() const { return bids_; }
    const auto &getAsks() const { return asks_; }
    std::size_t Size() const { return orders_.size(); }

    // public methods to add order, cancel order, and match order
    Trades AddOrder(OrderPtr order);
    void CancelOrder(OrderId orderId);
    Trades MatchOrders();
    Trades ModifyOrders(OrderModify mod); // cancel + add; allow user to change price and quantity
    OrderbookLevelInfos GetOrderInfos() const;
};