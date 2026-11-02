#include "../../../include/server/ai/GameView.hpp"

GameView::GameView(std::optional<Network::GameStatePayload>& g, Player& p){
    if(!g.has_value()) { 
        std::cout << "ERROR : no payload given to initialize GameView" << std::endl;
        return; 
    }

    for(auto data : g->cardsInShop){
        supplies[data.first] = data.second;
    }

    for(Network::PlayerState ps : g->players){
        PlayerView view = {ps.handSize, ps.drawStackSize, ps.discardStackSize, ps.victoryPoints};
        otherPlayersStats[ps.playerName.toStdString()] = view;
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

    purchases = g->buys;
    actions = g->actions;
    coins = g->money;
    currentPlayer = &p;
}