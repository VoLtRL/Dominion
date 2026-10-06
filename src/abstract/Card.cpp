#include "../../include/abstract/Card.hpp"
#include "CardType.hpp"

void Card:isA(CardType t){
    std::set<Card> types = this->getTypes();
    return types.find(t) != types.end();
}