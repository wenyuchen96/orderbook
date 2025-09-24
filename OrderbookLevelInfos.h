#pragma once

#include <vector>
#include "Usings.h"

struct LevelInfo
{
    Price price;
    Quantity totalQuantity;
};

class OrderbookLevelInfos
{
private:
    std::vector<LevelInfo> bids_;
    std::vector<LevelInfo> asks_;

public:
    OrderbookLevelInfos(std::vector<LevelInfo> bids, std::vector<LevelInfo> asks)
        : bids_(std::move(bids)), asks_(std::move(asks)) {};

    const auto &getBids() const { return bids_; }
    const auto &getAsks() const { return asks_; }
};