#pragma once

#include <vector>
#include <memory>

#include "../player/Player.hpp"
#include "GameMode.hpp"
#include "TableStack.hpp"

class Game {
    private:
        std::vector<Player> players;
        const static std::vector<GameMode> gameModes;
        std::vector<TableStack> KingdomStacks;
        std::vector<TableStack> BasicStacks;

        Player* activePlayer;
        const GameMode* currentGameMode;

        int currentPlayerMoney = 0;
        int currentPlayerActions = 0;
        int currentPlayerBuyChoices = 0;
        std::vector<const Card*> CardsInPlay;

        std::vector<const Card*> trash;

    public:
        Game();

        const std::vector<TableStack>& getKingdomStacks();
        const std::vector<TableStack>& getBasicStacks();

        void addPlayer(Player player);
        const std::vector<Player>& getPlayers() const;

        const Player& getActivePlayer() const;
        void setActivePlayer(Player* player);

        void setCurrentGameMode(const GameMode* gameMode);
        const GameMode* getCurrentGameMode() const;

        const std::vector<const Card*>& getCardsInPlay() const { return CardsInPlay; }
        void addCardToPlay(const Card* card) { CardsInPlay.push_back(card); }

        void ActionPhase(Player player);
        void BuyPhase(Player player);

        void playTurn(Player player);

        Player Game::getWinner();

        void launchGame();

        std::vector<std::string> parseLines(std::string);

        std::vector<const Card*> chooseCardsFromSelection(const std::vector<const Card*>& selection, size_t numberOfCardsToChoose=1,const std::string &action,std::string mode="exact") const;
        void displaySelection(const std::vector<const Card*>& selection) const;
};
