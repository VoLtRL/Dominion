#pragma once

#include <vector>
#include <memory>

#include "../player/Player.hpp"
#include "GameMode.hpp"

class Game {
    private:
        std::vector<Player> players;
        const static std::vector<GameMode> gameModes;

        Player* activePlayer;
        const GameMode* currentGameMode;

        int currentPlayerMoney = 0;
        int currentPlayerActions = 0;
        int currentPlayerBuyChoices = 0;
        std::vector<Card*> CardsInPlay;

    public:
        Game();

        void addPlayer(Player player);
        const std::vector<Player>& getPlayers() const;

        const Player& getActivePlayer() const;
        void setActivePlayer(Player* player);

        void setCurrentGameMode(const GameMode* gameMode);
        const GameMode* getCurrentGameMode() const;

        const std::vector<Card*>& getCardsInPlay() const { return CardsInPlay; }
        void addCardToPlay(Card* card) { CardsInPlay.push_back(card); }

        void playTurn(Player player);

        void launchGame();

};
