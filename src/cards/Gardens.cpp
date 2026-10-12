#include "../../include/gameplay/Game.hpp"
#include "../../include/cards/Gardens.hpp"
#include "../../include/player/Player.hpp"

void Gardens::onGain(Game& g, Player& p) const{
    // Calculate the number of cards in the player's deck
    int totalCards = p.getDrawStack().getSize() + p.getHand().getSize() + p.getDiscardStack().getSize();
    // Calculate the number of victory points based on the total number of cards
    int victoryPoints = totalCards / 10; // 1 VP per 10 cards, rounded down
    // Add the calculated victory points to the player's score
    p.addVictoryPoints(victoryPoints);
}