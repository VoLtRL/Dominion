#include "../../include/gameplay/Game.hpp"
#include <iostream>
#include <memory>
#include <algorithm>

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

std::vector<std::string> Game::parseLines(const std::string&){

}

void Game::displaySelection(const std::vector<const Card*>& selection) const {
    const std::string gap = "  ";
    const size_t cardsPerRow = 4;

    for (size_t start = 0; start < selection.size(); start += cardsPerRow) {
        const size_t end = std::min(start + cardsPerRow, selection.size());

        std::vector<std::vector<std::string>> cards;
        std::vector<size_t> widths;
        size_t maxHeight = 0;

        // Découper chaque carte en lignes
        for (size_t i = start; i < end; ++i) {
            std::vector<std::string> lines = parseLines(selection[i]->getString());
            widths.push_back(lines.empty() ? 0 : lines[0].size());
            maxHeight = std::max(maxHeight, lines.size());
            cards.push_back(std::move(lines));
        }

        // Étirer les cartes trop courtes : lignes vides avant la bordure basse
        for (size_t c = 0; c < cards.size(); ++c) {
            auto& lines = cards[c];
            if (lines.size() >= maxHeight || lines.size() < 2) continue;

            const size_t w = widths[c];
            // ligne vide : "|" + espaces + "|"
            const std::string emptyRow = "|" + std::string(w - 2, ' ') + "|";

            const size_t missing = maxHeight - lines.size();
            lines.insert(lines.end() - 1, missing, emptyRow);
        }

        // Affichage ligne par ligne
        for (size_t row = 0; row < maxHeight; ++row) {
            for (size_t c = 0; c < cards.size(); ++c) {
                if (row < cards[c].size())
                    std::cout << cards[c][row];
                else
                    std::cout << std::string(widths[c], ' ');

                if (c + 1 < cards.size())
                    std::cout << gap;
            }
            std::cout << '\n';
        }
        std::cout << '\n';
    }
}

std::vector<const Card*> Game::chooseCardsFromSelection(const std::vector<const Card*>& selection, size_t numberOfCardsToChoose,const std::string& action, std::string mode) const {
    if (mode != "exact" && mode != "most" && mode != "least") {
        mode = "exact";
    }

    if (mode != "most") {
        numberOfCardsToChoose = std::min(numberOfCardsToChoose, selection.size());
    }

    std::vector<const Card*> chosenCards;
    std::vector<bool> taken(selection.size(), false);

    if (mode == "exact"){
        std::cout << "Choose exactly " << numberOfCardsToChoose << " card(s) to " << action << ":\n";
    }
    else if (mode == "most"){
        std::cout << "Choose at most " << numberOfCardsToChoose << " card(s) (0 to finish) to " << action << ":\n";
    }
    else{
        std::cout << "Choose at least " << numberOfCardsToChoose << " card(s) (0 to finish once done) to " << action << ":\n";
    }

    displaySelection(selection);

    while (true) {
        const bool allTaken  = chosenCards.size() == selection.size();
        const bool reachedN  = chosenCards.size() >= numberOfCardsToChoose;

        // Arrêt automatique
        if (allTaken || (mode != "least" && reachedN)) break;

        const bool canStop = (mode == "most") || (mode == "least" && reachedN);

        std::cout << "[" << chosenCards.size() << " chosen] Enter a card number"
                  << (canStop ? " (0 to finish)" : "") << ": ";

        int index;
        if (!(std::cin >> index)) {
            if (std::cin.eof()){
                break;
            }
            std::cin.clear();
            std::cout << "Invalid input. Please try again.\n";
            continue;
        }

        if (index == 0 && canStop) break;

        if (index < 1 || index > selection.size()) {
            std::cout << "Invalid index. Please try again.\n";
            continue;
        }
        if (taken[index - 1]) {
            taken[index - 1] = false;
            auto chosen = std::find(chosenCards.begin(), chosenCards.end(), selection[index - 1]);
            if (chosen != chosenCards.end()) {
                chosenCards.erase(chosen);
            }
            std::cout << "Card deselected.\n";
            continue;
        }

        taken[index - 1] = true;
        chosenCards.push_back(selection[index - 1]);
    }

    return chosenCards;
}

void Game::ActionPhase(Player player) {
     while(currentPlayerActions > 0) {
        std::cout << "You have " << currentPlayerActions << " actions left." << std::endl;
        const std::vector<const Card*> cards = player.getHand().getCards();
        for (const Card* c : cards) {
            if (!c->isA(CardType::Action)) {
                std::remove(cards.begin(), cards.end(), c);
            }
        }
        if (cards.empty()) {
            std::cout << "You have no action cards to play." << std::endl;
            break;
        }
        auto card = chooseCardsFromSelection(cards, 1, "play", "exact")[0]
        if (card.empty()) {
            std::cout << "No card selected. Please try again." << std::endl;
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
