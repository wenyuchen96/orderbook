#pragma once

#include "Usings.h"
#include "Ordertype.h"
#include "Side.h"

#include <stdexcept>
#include <format>
#include <memory>
#include <list>

class Order
{
private:
    OrderType orderType_;
    OrderId orderId_;
    Side side_;
    Price price_;
    Quantity initialQty_;
    Quantity remainingQty_;

public:
    // constructor for limit orders: GTC, FAK, FOK, GFD
    Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity qty)
        : orderType_(orderType),
          orderId_(orderId),
          side_(side),
          price_(price),
          initialQty_(qty),
          remainingQty_(qty) {};

    // constructor for market orders (no price)
    Order(OrderType orderType, OrderId orderId, Side side, Quantity qty)
        : orderType_(orderType),
          orderId_(orderId),
          side_(side),
          initialQty_(qty),
          remainingQty_(qty) {};

    // getters
    OrderType getOrderType() const { return orderType_; }
    OrderId getOrderId() const { return orderId_; }
    Side getSide() const { return side_; }
    Price getPrice() const { return price_; }
    Quantity getInitialQuantity() const { return initialQty_; }
    Quantity getRemainingQuantity() const { return remainingQty_; }

    // helpers
    bool isFilled() const { return remainingQty_ == 0; };

    // mutators
    void Fill(Quantity q)
    {
        if (q > remainingQty_)
        {
            throw std::logic_error(
                std::format("Order {} cannot be filled for more than its remaining quantity {}", orderId_, remainingQty_));
        }
        remainingQty_ -= q;
    };

    // mutate market order into a GoodTillCancel limit order at the worst bid or ask price a market participant can take on the limit orderbook for execution
    void ToGoodTillCancel(Price newPrice)
    {
        if (orderType_ != OrderType::Market)
        {
            throw std::logic_error(std::format("Order {} cannot be convered to a GoodTillCancel order since it's not a market order", orderId_));
        }

        price_ = newPrice;
        orderType_ = OrderType::GoodTillCancel;
    };
};

// smart pointers at the Order object
using OrderPtr = std::shared_ptr<Order>;
using OrderPtrs = std::list<OrderPtr>;