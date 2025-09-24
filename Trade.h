#pragma once

#include "Usings.h"
#include <vector>

struct Trade
{
    OrderId buyId, sellId;
    Price price;
    Quantity qty;
};

using Trades = std::vector<Trade>;