#include "Orderbook.h"

#include <iostream>

int main()
{
    Orderbook ob;

    // Add a couple of limit orders
    auto buy1 = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Buy, 100, 50);
    auto buy2 = std::make_shared<Order>(OrderType::GoodTillCancel, 2, Side::Buy, 101, 30);

    auto sell1 = std::make_shared<Order>(OrderType::GoodTillCancel, 3, Side::Sell, 102, 40);
    auto sell2 = std::make_shared<Order>(OrderType::GoodTillCancel, 4, Side::Sell, 101, 20);

    ob.AddOrder(buy1);
    ob.AddOrder(buy2);
    ob.AddOrder(sell1);

    // Try matching (should match buy2 @101 with sell2 @101)
    ob.AddOrder(sell2);

    std::cout << "Orderbook size: " << ob.Size() << "\n";

    // Modify order #1 (change price from 100 -> 103, qty 50 -> 60)
    OrderModify mod(1, Side::Buy, 103, 60);
    auto trades = ob.ModifyOrders(mod);

    std::cout << "Trades executed after modify: " << trades.size() << "\n";
    for (auto &t : trades)
    {
        std::cout << "Trade: buyId=" << t.buyId
                  << " sellId=" << t.sellId
                  << " price=" << t.price
                  << " qty=" << t.qty << "\n";
    }

    // Cancel order #3
    try
    {
        ob.CancelOrder(3);
        std::cout << "Order 3 cancelled.\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Cancel failed: " << e.what() << "\n";
    }

    std::cout << "Final orderbook size: " << ob.Size() << "\n";
}