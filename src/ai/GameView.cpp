#include "../../include/ai/GameView.hpp"
#include "../../include/gameplay/Game.hpp"

GameView::GameView(const Game& g, Player& p){
    for(auto data : g.getCurrentCardDistrib()){
        supplies[data.first] = data.second;
    }

    for(Player* p : g.getOtherPlayers()){
        PlayerView view = {(int)p->getHand().getSize(), (int)p->getDrawStack().getSize(), (int)p->getDiscardStack().getSize(), p->getVictoryPoints()};
        otherPlayersStats[p] = view;
    }

    for(const Card* c : p.getHand().getCards()){
        cardCount[c]++;
    }

    for(auto data : p.getDrawStack().getCardCount()){
        cardCount[data.first] += data.second;
    }

    for(auto data : p.getDiscardStack().getCardCount()){
        cardCount[data.first] += data.second;
    }

    purchases = g.getPurchaseCount();
    actions = g.getActionCount();
    coins = g.getCoins();
    currentPlayer = &p;
}