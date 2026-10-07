#include "../../include/gameplay/Game.hpp"
#include <iostream>
#include <memory>

const std::vector<GameMode> Game::gameModes = {
    GameMode("Les premières parties", "Configuration recommandée pour découvrir le jeu", {
        {new Workshop(), 10},
        {new Woodcutter(), 10},
        {new Cellar(), 10},
        {new Moat(), 10},
        {new Smithy(), 10},
        {new Market(), 10},
        {new Militia(), 10},
        {new Mine(), 10},
        {new Remodel(), 10},
        {new Village(), 10}
    }),

    GameMode("Richesses et trésors", "Aventurier, Bureaucrate, Chancelier, Chapelle, Festin, Laboratoire, Marché, Mine, Prêteur sur gages, Salle du Trône", {
        {new Adventurer(), 10},
        {new Bureaucrat(), 10},
        {new Chancellor(), 10},
        {new Chapel(), 10},
        {new Feast(), 10},
        {new Laboratory(), 10},
        {new Market(), 10},
        {new Mine(), 10},
        {new Moneylender(), 10},
        {new ThroneRoom(), 10}
    }),

    GameMode("Interaction", "Bibliothèque, Bureaucrate, Chambre du conseil, Chancelier, Douves, Espion, Festival, Milice, Village, Voleur", {
        {new Library(), 10},
        {new Bureaucrat(), 10},
        {new CouncilRoom(), 10},
        {new Chancellor(), 10},
        {new Moat(), 10},
        {new Spy(), 10},
        {new Festival(), 10},
        {new Militia(), 10},
        {new Village(), 10},
        {new Thief(), 10}
    }),

    GameMode("Changement de taille", "Atelier, Bûcheron, Cave, Chapelle, Festin, Jardins, Laboratoire, Sorcière, Village, Voleur", {
        {new Workshop(), 10},
        {new Woodcutter(), 10},
        {new Cellar(), 10},
        {new Chapel(), 10},
        {new Feast(), 10},
        {new Gardens(), 12},
        {new Laboratory(), 10},
        {new Witch(), 10},
        {new Village(), 10},
        {new Thief(), 10}
    }),

    GameMode("Place du Village", "Bibliothèque, Bûcheron, Bureaucrate, Cave, Festival, Forgeron, Marché, Rénovation, Salle du Trône, Village", {
        {new Library(), 10},
        {new Woodcutter(), 10},
        {new Bureaucrat(), 10},
        {new Cellar(), 10},
        {new Festival(), 10},
        {new Smithy(), 10},
        {new Market(), 10},
        {new Remodel(), 10},
        {new ThroneRoom(), 10},
        {new Village(), 10}
    })
};

Game::Game() {
    
    activePlayer = nullptr;
}

const std::vector<Player>& Game::getPlayers() const {
    return players;
}

const Player& Game::getActivePlayer() const {
    return *activePlayer;
}

void Game::addPlayer(Player player) {
    players.push_back(player);
}

void Game::setActivePlayer(Player* player) {
    activePlayer = player;
}

void Game::setCurrentGameMode(const GameMode* gameMode) {
    currentGameMode = gameMode;
}

const GameMode* Game::getCurrentGameMode() const {
    return currentGameMode;
}

void Game::ActionPhase(Player player) {
     while(currentPlayerActions > 0) {
        std::cout << "You have " << currentPlayerActions << " actions left." << std::endl;
        std::cout << "Your hand: " << std::endl;
        player.getHand().displayHand();
        std::cout << "Enter the index of the card you want to play (or -1 to end your action phase): ";
        int index;
        std::cin >> index;
        if (index == -1)
            break;
        if (index < 0 || index >= player.getHand().getCards().size()) {
            std::cout << "Invalid index. Please try again." << std::endl;
            continue;
        }
        const Card* card = player.getHand().getCards()[index];
        if (!card->isA(CardType::Action)) {
            std::cout << "You can only play action cards during the action phase. Please try again." << std::endl;
            continue;
        }
        player.playCard(card, *this);
        currentPlayerActions--; // je sais pas
    }
    std::cout << "Action phase ended." << std::endl;

}

void Game::BuyPhase(Player player) {
    // to do
}

void Game::playTurn(Player player) {
    setActivePlayer(&player);
    player.addTurnPlayed();
    ActionPhase(player);
    BuyPhase(player);
}

Player Game::getWinner() {
    Player winner = players[0];
    int maxScore = 0;

    for (const Player& player : players) {
        if (player.getScore() > maxScore) {
            maxScore = player.getScore();
            winner = player;
        }
    }

    return winner;
}

void Game::launchGame() {
    std::cout << "Welcome to Dominion!" << std::endl;
    std::cout << "Available game modes:" << std::endl;
    for (size_t i = 0; i < gameModes.size(); ++i) {
        std::cout << i + 1 << ". " << gameModes[i].getName() << " - " << gameModes[i].getDescription() << std::endl;
    }
    std::cout << "Please select a game mode by entering the corresponding number: ";
    int choice;
    std::cin >> choice;
    if (choice < 1 || choice > gameModes.size()) {
        std::cout << "Invalid choice." << std::endl;
        return;
    }
    setCurrentGameMode(&gameModes[choice - 1]);

    std::cout << "Will you play alone ? Enter the number of players (4 players max) [default : 1 player]" << std::endl;
    choice = 1;
    std::cin >> choice;
    if (choice < 1 || choice > 4){
        std::cout << "Invalid choice." << std::endl;
    }
    std::cout << "You have chosen to play with " << choice << " players." << std::endl;
    for (int i = 0; i < choice; ++i) {
        std::cout << "Enter the name of player " << i + 1 << ": ";
        std::string playerName;
        std::cin >> playerName;
        CardStack drawStack;

        drawStack.shuffle();
        Hand hand;
        CardStack discardStack = CardStack();
        hand.refillHandFromDrawStack(drawStack, discardStack);
        Player player(hand, drawStack, discardStack);
        addPlayer(player);
    }

    while (true) {
        for (Player& player : players) {
            playTurn(player);
        }
    }

    std::cout << "Game is over!" << std::endl;
    std::cout << "The winner is:" << getWinner().getString() << std::endl;
    
}
