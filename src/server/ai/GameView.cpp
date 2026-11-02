#include "../../../include/server/ai/GameView.hpp"
#include "../../../include/shared/gameplay/Game.hpp"

GameView::GameView(const Game& g, Player& p){
    for(auto data : g.getCurrentCardDistrib()){
        supplies[data.first] = data.second;
    }

    for(const std::unique_ptr<Player>& player : g.getPlayers()){
        PlayerView view = {(int)player->getHand().getSize(), (int)player->getDrawStack().getSize(), (int)player->getDiscardStack().getSize(), player->getVictoryPoints()};
        otherPlayersStats[player.get()] = view;
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

    purchases = g.getCurrentPlayerPurchasesLeft();
    actions = g.getCurrentPlayerActions();
    coins = g.getCurrentPlayerMoney();
    currentPlayer = &p;
}