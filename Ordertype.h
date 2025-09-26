#pragma once

enum class OrderType
{
    GoodTillCancel,    // "GTC", a standard order that remains active until it is fully filled or manually canceled.
    ImmediateOrCancel, // "FOK", an order that must be entirely filled immediately. If not, the entire order is canceled.
    FillOrKill,
    GoodForDay, // "GFD", an order that remains active until the end of the trading day. Any unfilled portion is canceled at that time.
    Market      // "Market", an order to buy or sell at the best available price in the market. It does not specify a price.
};