#include "client/MainWindow.hpp"

#include "client/CreateGame.hpp"
#include "client/GameLobby.hpp"
#include "client/GameTable.hpp"
#include "client/JoinRoom.hpp"
#include "client/MainMenu.hpp"
#include "shared/structs/Payloads.hpp"
#include "client/ClientPreferences.hpp"
#include "ui_MainWindow.h"

#include <QAbstractSocket>

MainWindow::MainWindow(QWidget *parent, NetworkManager *networkManager, ClientPreferences *clientPreferences)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      networkManager(networkManager),
      clientPreferences(clientPreferences),
      mainMenu(nullptr),
      createGame(nullptr),
      gameLobby(nullptr),
      gameTable(nullptr),
      joinRoom(nullptr) {
    ui->setupUi(this);
    reconnectTimer.setInterval(1000);
    connect(&reconnectTimer, &QTimer::timeout, this, &MainWindow::tryReconnect);

    createPages();
    mainMenu->setSettings(
        clientPreferences->getPlayerName(),
        clientPreferences->getHostAddress(),
        clientPreferences->getHostPort());
    gameLobby->setCurrentPlayerName(clientPreferences->getPlayerName());
    connectNavigation();
    connectNetwork();
    connectGameTable();
    showPage(Page::MainMenu);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::createPages() {
    mainMenu = new MainMenu(this);
    createGame = new CreateGame(this);
    gameLobby = new GameLobby(this);
    gameTable = new GameTable(this);
    joinRoom = new JoinRoom(this);

    ui->stackedWidget->addWidget(mainMenu);
    ui->stackedWidget->addWidget(createGame);
    ui->stackedWidget->addWidget(gameLobby);
    ui->stackedWidget->addWidget(gameTable);
    ui->stackedWidget->addWidget(joinRoom);
}

void MainWindow::connectNavigation() {
    connect(mainMenu, &MainMenu::createGameClicked, this, &MainWindow::showCreateGame);
    connect(mainMenu, &MainMenu::joinRoomClicked, this, &MainWindow::showJoinRoom);
    connect(mainMenu, &MainMenu::quitClicked, this, &MainWindow::close);
    connect(mainMenu, &MainMenu::settingsChanged, this, &MainWindow::applySettings);

    connect(createGame, &CreateGame::createRoomClicked, this, &MainWindow::createRoom);
    connect(createGame, &CreateGame::goBackToMenuClicked, this, &MainWindow::showMainMenu);
    connect(joinRoom, &JoinRoom::joinRoomClicked, this, &MainWindow::joinRoomById);
    connect(joinRoom, &JoinRoom::goBackToMenuClicked, this, &MainWindow::showMainMenu);
    connect(gameLobby, &GameLobby::goBackToMenuClicked, this, [this]() {
        leaveRoom();
        showMainMenu();
    });
    connect(gameLobby, &GameLobby::launchGameClicked, this, [this]() {
        if (ensureConnected()) {
            networkManager->sendAction(ActionType::START_GAME, Network::EmptyPayload());
        }
    });
}

void MainWindow::connectNetwork() {
    connect(networkManager, &NetworkManager::connected, this, &MainWindow::handleConnected);
    connect(networkManager, &NetworkManager::disconnected, this, &MainWindow::handleDisconnected);
    connect(networkManager, &NetworkManager::actionReceived, this, &MainWindow::handleAction);
    connect(networkManager, &NetworkManager::connectionError, this, [this](const QString &error) {
        ui->statusbar->showMessage(error);
        if (networkManager->getState() != QAbstractSocket::ConnectedState) {
            reconnectTimer.start();
        }
    });
}

void MainWindow::connectGameTable() {
    connect(gameTable, &GameTable::endTurnClicked, this, [this]() {
        if (ensureConnected()) {
            networkManager->sendAction(ActionType::END_TURN, Network::EmptyPayload());
        }
    });
    connect(gameTable, &GameTable::phaseChanged, this, [this](GamePhase newPhase) {
        if (ensureConnected()) {
            ui->statusbar->showMessage(QString("Phase changed to: %1").arg(toString(newPhase).c_str()));
            gameTable->addLogMessage(gameTable->getActivePlayerName(), Qt::darkBlue, QString("Phase changed to: %1").arg(toString(newPhase).c_str()));
        } else {
            gameTable->displayMessage("Not connected to server. Cannot change phase.");
        }
    });

    connect(gameTable, &GameTable::cardPlayed, this, &MainWindow::handlePlayCard);
    connect(gameTable, &GameTable::nextPhaseClicked, this, [this]() {
        if (ensureConnected()) {
            qDebug() << "Sending NEXT_PHASE action to server.";
            networkManager->sendAction(ActionType::NEXT_PHASE, Network::EmptyPayload());
        } else {
            gameTable->displayMessage("Not connected to server. Cannot proceed to next phase.");
        }
    });
    connect(gameTable, &GameTable::cardBought, this, &MainWindow::handleBuyCard);
    connect(gameTable, &GameTable::allTreasuresPlayed, this, &MainWindow::handleAllTreasuresPlayed);
    connect(gameTable, &GameTable::selectionSubmitted, this, [this](const std::vector<const Card*> &cards) {
        networkManager->sendAction(ActionType::SELECTION_REQUEST,
                                    Network::SelectionResponsePayload(cards));
    });

    connect(gameTable, &GameTable::reactionPlayed, this, [this](const Card *card) {
        networkManager->sendAction(ActionType::REACTION,
                                    Network::ReactionResponsePayload(clientPreferences->getPlayerName(), true, card));
    });

    connect(gameTable, &GameTable::reactionPassed, this, [this]() {
        networkManager->sendAction(ActionType::REACTION,
                                    Network::ReactionResponsePayload(clientPreferences->getPlayerName(), false, nullptr));
    });
}

void MainWindow::showPage(Page page) {
    switch (page) {
    case Page::MainMenu:
        ui->stackedWidget->setCurrentWidget(mainMenu);
        break;
    case Page::CreateGame:
        ui->stackedWidget->setCurrentWidget(createGame);
        break;
    case Page::GameLobby:
        ui->stackedWidget->setCurrentWidget(gameLobby);
        break;
    case Page::GameTable:
        ui->stackedWidget->setCurrentWidget(gameTable);
        break;
    case Page::JoinRoom:
        ui->stackedWidget->setCurrentWidget(joinRoom);
        break;
    }
}

bool MainWindow::ensureConnected() {
    if (networkManager->getState() == QAbstractSocket::ConnectedState) {
        return true;
    }

    ui->statusbar->showMessage("Not connected to server. Reconnecting...");
    tryReconnect();
    return false;
}

void MainWindow::showCreateGame() {
    if (ensureConnected()) {
        showPage(Page::CreateGame);
    }
}

void MainWindow::showJoinRoom() {
    if (!ensureConnected()) {
        return;
    }

    showPage(Page::JoinRoom);
    networkManager->sendAction(ActionType::REQUEST_ROOM_LIST, Network::EmptyPayload());
}

void MainWindow::showMainMenu() {
    gameStartReceived = false;
    initialGameStateReceived = false;
    initialTurnStarted = false;
    showPage(Page::MainMenu);
}

void MainWindow::leaveRoom() {
    if (!clientPreferences->getCurrentRoomId().has_value()) {
        return;
    }

    if (ensureConnected()) {
        networkManager->sendAction(ActionType::LEAVE_ROOM, Network::EmptyPayload());
    }

    clientPreferences->setCurrentRoomId(std::nullopt);
}

void MainWindow::createRoom() {
    if (!ensureConnected()) {
        return;
    }

    Network::CreateRoomPayload roomData = createGame->getRoomData();
    roomData.playerName = clientPreferences->getPlayerName();
    networkManager->sendAction(ActionType::CREATE_ROOM, roomData);
}

void MainWindow::joinRoomById(int roomId) {
    if (!ensureConnected()) {
        return;
    }

    networkManager->sendAction(
        ActionType::JOIN_ROOM,
        Network::JoinRoomPayload(roomId, clientPreferences->getPlayerName()));
}

void MainWindow::handleConnected() {
    reconnectTimer.stop();
    ui->statusbar->showMessage(
        QString("Connected to %1:%2").arg(clientPreferences->getHostAddress()).arg(clientPreferences->getHostPort()));
}

void MainWindow::handleDisconnected() {
    ui->statusbar->showMessage("Disconnected from server. Reconnecting...");
    reconnectTimer.start();
}

void MainWindow::tryReconnect() {
    if (networkManager->getState() == QAbstractSocket::ConnectedState ||
        networkManager->getState() == QAbstractSocket::ConnectingState) {
        return;
    }

    networkManager->connectToServer();
}

void MainWindow::handleAction(const ActionType &action, const QJsonObject &data) {
    switch (action) {
    case ActionType::ROOM_CREATED:
        handleRoomCreated(data);
        break;
    case ActionType::ROOM_CREATION_FAILED:
        handleRoomCreationFailed(data);
        break;
    case ActionType::ROOM_JOIN_FAILED:
        handleRoomJoinFailed(data);
        break;
    case ActionType::PLAYER_JOINED:
        handlePlayerJoined(data);
        break;
    case ActionType::JOIN_ROOM:
        handleRoomJoined(data);
        break;
    case ActionType::PLAYER_LEFT:
        handlePlayerLeft(data);
        break;
    case ActionType::START_GAME:
        handleGameStarted(data);
        break;
    case ActionType::TURN_STATE:
        handleTurnState(data);
        break;
    case ActionType::GAME_STATE:
        handleGameState(data);
        break;
    case ActionType::PLAY_CARD:
        handleCardPlayed(data);
        break;
    case ActionType::BUY_CARD:
        handleCardBought(data);
        break;
    case ActionType::GAINED_MONEY:
        handleGainedMoney(data);
        break;
    case ActionType::END_TURN:
        handleTurnEnded(data);
        break;
    case ActionType::NEXT_PHASE:
        handlePhaseChanged(data);
        break;
    case ActionType::GAME_ENDED: {
        const auto payload = Network::GameEndedPayload::fromJson(data);
        if (!payload) {
            qWarning() << "Invalid GAME_ENDED payload:" << data;
            break;
        }
        gameTable->handleGameEnded(payload->winnerName);
        break;
    }
    case ActionType::SELECTION_REQUEST:
        handleSelectionRequest(data);
        break;
    case ActionType::REACTION:
        handleReaction(data);
        break;
    case ActionType::ERROR: {
        const auto payload = Network::ErrorPayload::fromJson(data);
        if (!payload) {
            qWarning() << "Invalid ERROR payload:" << data;
            break;
        }
        gameTable->handleCardPlayRejected(payload->message);
        break;
    }
    case ActionType::REQUEST_ROOM_LIST:
        handleRoomList(data);
        break;
    default:
        qWarning() << "Unhandled server action:" << static_cast<int>(action);
        break;
    }
}

void MainWindow::handleRoomCreated(const QJsonObject &data) {
    const auto payload = Network::CreatedRoomPayload::fromJson(data);
    if (!payload) {
        qWarning() << "Invalid ROOM_CREATED payload:" << data;
        return;
    }

    clientPreferences->setCurrentRoomId(payload->roomId);

    gameLobby->setRoomName(payload->roomName);
    gameLobby->setCurrentPlayerName(clientPreferences->getPlayerName());
    gameLobby->clearPlayerList();
    gameLobby->addPlayerToList(clientPreferences->getPlayerName());

    gameLobby->updateRoomInfo();
    showPage(Page::GameLobby);
}

void MainWindow::handleRoomCreationFailed(const QJsonObject &data) {
    ui->statusbar->showMessage("Room creation failed: " + data.value("error").toString());
}

void MainWindow::handleRoomJoinFailed(const QJsonObject &data) {
    ui->statusbar->showMessage("Room join failed: " + data.value("error").toString());
}

void MainWindow::handlePlayerJoined(const QJsonObject &data) {
    const auto payload = Network::PlayerJoinedRoomPayload::fromJson(data);
    if (!payload) {
        qWarning() << "Invalid PLAYER_JOINED payload:" << data;
        return;
    }

    gameLobby->addPlayerToList(payload->playerName);
}

void MainWindow::handleRoomJoined(const QJsonObject &data) {
    const auto payload = Network::RoomJoinedPayload::fromJson(data);
    if (!payload) {
        qWarning() << "Invalid JOIN_ROOM payload:" << data;
        return;
    }
    clientPreferences->setCurrentRoomId(payload->roomInfo.roomId);

    gameLobby->setRoomName(payload->roomInfo.roomName);
    gameLobby->setCurrentPlayerName(clientPreferences->getPlayerName());
    gameLobby->clearPlayerList();
    for (const auto &playerName : payload->playerNames) {
        gameLobby->addPlayerToList(playerName);
    }
    gameLobby->updateRoomInfo();
    showPage(Page::GameLobby);
}

void MainWindow::handlePlayerLeft(const QJsonObject &data) {
    const auto payload = Network::PlayerLeftRoomPayload::fromJson(data);
    if (!payload) {
        qWarning() << "Invalid PLAYER_LEFT payload:" << data;
        return;
    }

    gameLobby->removePlayerFromList(payload->playerName);
    gameLobby->updateRoomInfo();
}

void MainWindow::handleGameStarted(const QJsonObject &data) {
    const auto payload = Network::StartGamePayload::fromJson(data);
    if (!payload) {
        qWarning() << "Invalid START_GAME payload:" << data;
        return;
    }

    gameStartReceived = true;
    initialTurnStarted = false;
    showPage(Page::GameTable);
    startTurnWhenReady();
}

void MainWindow::handleTurnState(const QJsonObject &data) {
    const auto payload = Network::TurnStatePayload::fromJson(data);
    if (!payload) {
        qWarning() << "Invalid TURN_STATE payload:" << data;
        return;
    }
}

void MainWindow::handleGameState(const QJsonObject &data) {
    const auto payload = Network::GameStatePayload::fromJson(data);
    if (!payload) {
        qWarning() << "Invalid GAME_STATE payload:" << data;
        return;
    }

    updateGameState(*payload);
}

void MainWindow::handlePhaseChanged(const QJsonObject &data) {
    const auto payload = Network::PhaseChangedPayload::fromJson(data);
    if (!payload) {
        qWarning() << "Invalid NEXT_PHASE payload:" << data;
        return;
    }

    gameTable->updatePhase(payload->newPhase);
}

void MainWindow::updateGameState(const Network::GameStatePayload &payload) {
    ClientGameState instance;

    instance.selfName = payload.selfName;
    instance.activePlayerName = payload.activePlayerName;
    instance.currentPhase = payload.currentPhase;
    instance.currentPendingInteractionPhase = payload.pendingInteractionPhase;

    instance.currentPlayerActions = payload.actions;
    instance.currentPlayerPurchasesLeft = payload.buys;
    instance.currentPlayerMoney = payload.money;

    instance.hand = payload.hand;

    instance.cardsInPlay = payload.cardsInPlay;
    instance.shop = payload.cardsInShop;

    instance.players.clear();
    for (const auto &playerState : payload.players) {
        ClientPlayerState clientPlayerState;

        clientPlayerState.name = playerState.playerName;
        clientPlayerState.isYourself = playerState.playerName == payload.selfName;

        clientPlayerState.handSize = playerState.handSize;
        clientPlayerState.drawStackSize = playerState.drawStackSize;

        clientPlayerState.discardStackSize = playerState.discardStackSize;
        clientPlayerState.topDiscardCard = playerState.topDiscardCard;

        clientPlayerState.cardsInPlay = playerState.cardsInPlay;

        clientPlayerState.isActive = playerState.isActive;

        instance.players.push_back(clientPlayerState);
    }

    gameTable->updateGameState(instance);
    initialGameStateReceived = true;
    startTurnWhenReady();
}

void MainWindow::startTurnWhenReady() {
    if (gameStartReceived && initialGameStateReceived && !initialTurnStarted) {
        initialTurnStarted = true;
        gameTable->startTurn();
        gameTable->addLogMessage("Server", Qt::yellow, "the game has started");
    }
}

void MainWindow::handleRoomList(const QJsonObject &data) {
    const auto payload = Network::RoomListPayload::fromJson(data);
    if (!payload) {
        qWarning() << "Invalid REQUEST_ROOM_LIST payload:" << data;
        return;
    }

    joinRoom->updateAvailableRooms(payload->rooms);
}

void MainWindow::handleCardPlayed(const QJsonObject &data) {
    const auto payload = Network::CardPlayed::fromJson(data);
    if (!payload) {
        qWarning() << "Invalid PLAY_CARD payload:" << data;
        return;
    }

    gameTable->handleCardPlayed(payload->card);
    gameTable->addLogMessage(gameTable->getActivePlayerName(), Qt::green, QString("played %1").arg(payload->card->getName().c_str()));
}

void MainWindow::handleGainedMoney(const QJsonObject &data) {
    const auto payload = Network::GainedMoneyPayload::fromJson(data);
    if (!payload) {
        qWarning() << "Invalid GAINED_MONEY payload:" << data;
        return;
    }

    gameTable->addLogMessage(gameTable->getActivePlayerName(), Qt::yellow, QString("gained %1 money").arg(payload->amount));
}

void MainWindow::handleSelectionRequest(const QJsonObject &data) {
    const auto payload = Network::SelectionRequestPayload::fromJson(data);
    if (!payload) {
        qWarning() << "Invalid SELECTION_REQUEST payload:" << data;
        return;
    }
    gameTable->handleSelectionRequest(payload->mode, payload->constraint, payload->numberOfCards, payload->cards);
}

void MainWindow::handlePlayCard(const Card *card) {
    if (!card) {
        return;
    }

    if (ensureConnected()) {
        networkManager->sendAction(ActionType::PLAY_CARD,Network::PlayCardPayload(card));
    }
}

void MainWindow::handleAllTreasuresPlayed() {
    if (ensureConnected()) {
        networkManager->sendAction(ActionType::PLAY_ALL_TREASURES, Network::EmptyPayload());
    }
}

void MainWindow::handleBuyCard(const Card *card) {
    if (card == nullptr) {
        return;
    }

    if (ensureConnected()) {
        networkManager->sendAction(ActionType::BUY_CARD,Network::BuyCardPayload(card));
    }
    
}

void MainWindow::handleCardBought(const QJsonObject &data) {
    const auto payload = Network::CardBought::fromJson(data);
    if (!payload) {
        qWarning() << "Invalid BUY_CARD payload:" << data;
        return;
    }
    const Card *card = payload->card;
    gameTable->handleCardBought(card);
    gameTable->addLogMessage(gameTable->getActivePlayerName(), Qt::GlobalColor::green, QString("bought %1").arg(card->getName().c_str()));
}

void MainWindow::handleReaction(const QJsonObject &data){
    if (data.contains("reactions")) {
        const auto payload = Network::ReactionPayload::fromJson(data);
        if (!payload) {
            qWarning() << "Invalid REACTION request payload:" << data;
            return;
        }
        gameTable->handleReactionRequest(payload->reactions);
        return;
    }

    const auto payload = Network::ReactionResponsePayload::fromJson(data);
    if (!payload) {
        qWarning() << "Invalid REACTION response payload:" << data;
        return;
    }
    if (payload->playerName == clientPreferences->getPlayerName()) {
        gameTable->handleReactionCompleted();
    }
    gameTable->displayMessage(payload->isReacting
        ? QString("%1 played a reaction.").arg(payload->playerName)
        : QString("%1 passed the reaction.").arg(payload->playerName));
    gameTable->addLogMessage(payload->playerName, Qt::GlobalColor::magenta, payload->isReacting ? "played a reaction" : "passed the reaction");
}

void MainWindow::handleTurnEnded(const QJsonObject &data) {
    const auto payload = Network::TurnEndedPayload::fromJson(data);
    if (!payload) {
        qWarning() << "Invalid END_TURN payload:" << data;
        return;
    }
    gameTable->displayMessage(QString("Turn ended. Next player: %1").arg(payload->nextPlayerName));
    gameTable->addLogMessage("Server", Qt::yellow, QString("Turn ended. Next player: %1").arg(payload->nextPlayerName));
    gameTable->clearCardsInPlay();
}

void MainWindow::applySettings(
    const QString &newPlayerName,
    const QString &hostAddress,
    quint16 hostPort) {
    clientPreferences->setPlayerName(newPlayerName);
    gameLobby->setCurrentPlayerName(clientPreferences->getPlayerName());
    clientPreferences->setHostAddress(hostAddress);
    clientPreferences->setHostPort(hostPort);

    reconnectTimer.stop();
    networkManager->disconnectFromServer();
    networkManager->connectToServer();
    ui->statusbar->showMessage(
        QString("Connection settings updated: %1:%2")
            .arg(clientPreferences->getHostAddress())
            .arg(clientPreferences->getHostPort()));
}
