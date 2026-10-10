#pragma once

#include <stdexcept>
#include "../abstract/Card.hpp"

class TableStack {
private:
Card card;
int quantity;
public:
    TableStack(Card card, int quantity) : card(card), quantity(quantity) {}

    const Card* getCard() const { return &card; }
    int getQuantity() const { return quantity; }
    void decreaseQuantity(int amount);
};
