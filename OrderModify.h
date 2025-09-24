#pragma once

#include "Usings.h"
#include "Side.h"

// an object that store the information about how the existing order will be modified
class OrderModify
{
private:
    OrderId orderId_;
    Side side_;
    Price newPrice_;
    Quantity newQty_;

public:
    OrderModify(OrderId orderId, Side side, Price newPrice, Quantity newQty)
        : orderId_(orderId),
          side_(side),
          newPrice_(newPrice),
          newQty_(newQty) {};

    OrderId getOrderId() const { return orderId_; }
    Side getSide() const { return side_; }
    Price getNewPrice() const { return newPrice_; }
    Quantity getNewQty() const { return newQty_; }
};