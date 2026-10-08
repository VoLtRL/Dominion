#include "../../include/gameplay/TableStack.hpp"

void TableStack::decreaseQuantity(int amount){
     {
        if (amount <= quantity) {
            quantity -= amount;
        } else {
            throw std::runtime_error("Not enough cards in the stack to decrease by the specified amount.");
        }
    }
}
