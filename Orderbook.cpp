#include "Orderbook.h"
#include "Order.h"
#include <stdexcept>

Trades Orderbook::AddOrder(OrderPtr order)
{
    if (orders_.contains(order->getOrderId()))
        throw std::runtime_error("Duplicate order ID");

    Trades trades;

    // 1. Market Orders
    if (order->getOrderType() == OrderType::Market)
    {
        if (order->getSide() == Side::Buy)
        {
            while (order->getRemainingQuantity() > 0 && !asks_.empty())
            {
                auto &bestAsk = *asks_.begin();
                auto askOrder = bestAsk.second.front();
                auto askId = askOrder->getOrderId();

                Quantity qty = std::min(order->getRemainingQuantity(), askOrder->getRemainingQuantity());
                order->Fill(qty);
                askOrder->Fill(qty);

                trades.push_back({order->getOrderId(), askOrder->getOrderId(), bestAsk.first, qty});

                if (askOrder->isFilled())
                {
                    bestAsk.second.pop_front();
                    orders_.erase(askId);
                    if (bestAsk.second.empty())
                        asks_.erase(bestAsk.first);
                }
                else
                {
                    auto entryIt = orders_.find(askId);
                    if (entryIt != orders_.end())
                        entryIt->second.location_ = bestAsk.second.begin();
                }
            }
        }
        else
        { // Market Sell
            while (order->getRemainingQuantity() > 0 && !bids_.empty())
            {
                auto &bestBid = *bids_.begin();
                auto bidOrder = bestBid.second.front();
                auto bidId = bidOrder->getOrderId();

                Quantity qty = std::min(order->getRemainingQuantity(), bidOrder->getRemainingQuantity());
                order->Fill(qty);
                bidOrder->Fill(qty);

                trades.push_back({bidOrder->getOrderId(), order->getOrderId(), bestBid.first, qty});

                if (bidOrder->isFilled())
                {
                    bestBid.second.pop_front();
                    orders_.erase(bidId);
                    if (bestBid.second.empty())
                        bids_.erase(bestBid.first);
                }
                else
                {
                    auto entryIt = orders_.find(bidId);
                    if (entryIt != orders_.end())
                        entryIt->second.location_ = bestBid.second.begin();
                }
            }
        }
        return trades;
    }

    // 2. Immediate-Or-Cancel (IOC)
    if (order->getOrderType() == OrderType::ImmediateOrCancel)
    {

        if (order->getSide() == Side::Buy)
            bids_[order->getPrice()].push_back(order);
        else
            asks_[order->getPrice()].push_back(order);

        trades = MatchOrders();

        if (!order->isFilled())
            CancelOrder(order->getOrderId());

        return trades;
    }

    // 3. Fill-Or-Kill (FOK)
    if (order->getOrderType() == OrderType::FillOrKill)
    {
        Quantity needed = order->getRemainingQuantity();
        Quantity available = 0;

        if (order->getSide() == Side::Buy)
        {
            for (auto &[price, bucket] : asks_)
            {
                if (price > order->getPrice())
                    break;
                for (auto &o : bucket)
                {
                    available += o->getRemainingQuantity();
                    if (available >= needed)
                        break;
                }
                if (available >= needed)
                    break;
            }
        }
        else
        {
            for (auto &[price, bucket] : bids_)
            {
                if (price < order->getPrice())
                    break;
                for (auto &o : bucket)
                {
                    available += o->getRemainingQuantity();
                    if (available >= needed)
                        break;
                }
                if (available >= needed)
                    break;
            }
        }

        if (available < needed)
            return {};
    }

    // 4. Limit or GTC orders
    OrderPtrs::iterator it;
    if (order->getSide() == Side::Buy)
    {
        auto &bucket = bids_[order->getPrice()];
        bucket.push_back(order);
        it = std::prev(bucket.end());
    }
    else
    {
        auto &bucket = asks_[order->getPrice()];
        bucket.push_back(order);
        it = std::prev(bucket.end());
    }

    orders_[order->getOrderId()] = {order, it};

    auto newTrades = MatchOrders();
    trades.insert(trades.end(), newTrades.begin(), newTrades.end());

    return trades;
};

void Orderbook::CancelOrder(OrderId orderId)
{
    auto it = orders_.find(orderId);
    if (it == orders_.end())
        throw std::runtime_error("Order not found");

    auto &entry = it->second;
    OrderPtr order = entry.order_;

    if (order->getSide() == Side::Buy)
    {
        auto &bucket = bids_[order->getPrice()];
        bucket.erase(entry.location_);
        if (bucket.empty())
            bids_.erase(order->getPrice());
    }
    else
    {
        auto &bucket = asks_[order->getPrice()];
        bucket.erase(entry.location_);
        if (bucket.empty())
            asks_.erase(order->getPrice());
    }

    orders_.erase(it);
};

Trades Orderbook::MatchOrders()
{
    Trades trades;

    while (!bids_.empty() && !asks_.empty())
    {
        auto &bestBid = *bids_.begin(); // highest buy
        auto &bestAsk = *asks_.begin(); // lowest ask

        Price bidPrice = bestBid.first;
        Price askPrice = bestAsk.first;
        if (bidPrice < askPrice)
            break; // no match

        auto &bidOrder = bestBid.second.front();
        auto &askOrder = bestAsk.second.front();
        auto bidId = bidOrder->getOrderId();
        auto askId = askOrder->getOrderId();

        Quantity qty = std::min(bidOrder->getRemainingQuantity(), askOrder->getRemainingQuantity());
        bidOrder->Fill(qty);
        askOrder->Fill(qty);

        trades.push_back({bidOrder->getOrderId(), askOrder->getOrderId(), askPrice, qty});

        if (bidOrder->isFilled())
        {
            bestBid.second.pop_front();
            orders_.erase(bidId);
            if (bestBid.second.empty())
                bids_.erase(bidPrice);
        }
        else
        {
            auto entryIt = orders_.find(bidId);
            if (entryIt != orders_.end())
                entryIt->second.location_ = bestBid.second.begin();
        }
        if (askOrder->isFilled())
        {
            bestAsk.second.pop_front();
            orders_.erase(askId);
            if (bestAsk.second.empty())
                asks_.erase(askPrice);
        }
        else
        {
            auto entryIt = orders_.find(askId);
            if (entryIt != orders_.end())
                entryIt->second.location_ = bestAsk.second.begin();
        }
    }

    return trades;
};

Trades Orderbook::ModifyOrders(OrderModify mod)
{
    auto it = orders_.find(mod.getOrderId());
    if (it == orders_.end())
        throw std::runtime_error("Order not found for modification");

    OrderType originalType = it->second.order_->getOrderType();
    CancelOrder(mod.getOrderId());
    auto newOrder = mod.ToOrderPointer(originalType);

    return AddOrder(newOrder);
}
