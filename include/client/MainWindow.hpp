#pragma once
#include <QMainWindow>
#include <QTimer>
#include <QJsonObject>
#include <QString>

#include "client/network/NetworkManager.hpp"
#include "shared/gameplay/Game.hpp"
#include "client/Game/ClientGameState.hpp"

class MainMenu;
class CreateGame;
class GameLobby;
class GameTable;
class JoinRoom;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    enum class Page {
        MainMenu,
        CreateGame,
        GameLobby,
        GameTable,
        JoinRoom
    };

    Ui::MainWindow *ui;
    NetworkManager *networkManager;
    ClientPreferences *clientPreferences;
    MainMenu *mainMenu;
    CreateGame *createGame;
    GameLobby *gameLobby;
    GameTable *gameTable;
    JoinRoom *joinRoom;
    QTimer reconnectTimer;
    bool gameStartReceived{false};
    bool initialGameStateReceived{false};
    bool initialTurnStarted{false};

private slots:
    void applySettings(const QString &newPlayerName, const QString &hostAddress, quint16 hostPort);
    void showCreateGame();
    void showJoinRoom();
    void showMainMenu();
    void createRoom();
    void joinRoomById(int roomId);
    void handleAction(const ActionType &action, const QJsonObject &data);
    void handleConnected();
    void handleDisconnected();
    void tryReconnect();

public:
    explicit MainWindow(QWidget *parent = nullptr, NetworkManager *networkManager = nullptr, ClientPreferences *clientPreferences = nullptr);
    ~MainWindow() override;

private:
    void createPages();
    void showPage(Page page);

    void connectNavigation();
    void connectNetwork();

    void connectGameTable();

    bool ensureConnected();
    void leaveRoom();

    void handleRoomCreated(const QJsonObject &data);
    void handleRoomCreationFailed(const QJsonObject &data);
    void handleRoomJoinFailed(const QJsonObject &data);

    void handlePlayerJoined(const QJsonObject &data);
    void handleRoomJoined(const QJsonObject &data);
    void handlePlayerLeft(const QJsonObject &data);

    void handleGameStarted(const QJsonObject &data);

    void handleRoomList(const QJsonObject &data);

    void handleTurnState(const QJsonObject &data);
    void handleGameState(const QJsonObject &data);


    /// @brief Handles the server's request to play a card. This function is called when the server sends a PLAY_CARD action.
    /// @param data The data associated with the PLAY_CARD action, which should contain the card to be played.
    void handleCardPlayed(const QJsonObject &data);
    /// @brief Handles the client's request to play a card. This function is called when the client plays a card in the game table.
    /// @param card The card that the client wants to play. This function sends a PLAY_CARD action to the server with the card information.
    void handlePlayCard(const Card *card);

    /// @brief  Handles the client's request to buy a card. This function is called when the client buys a card in the game table.
    /// @param card The card that the client wants to buy. This function sends a BUY_CARD action to the server with the card information.
    void handleBuyCard(const Card *card);
    /// @brief Handles the client's request to buy a card. This function is called when the client buys a card in the game table.
    /// @param data The data associated with the BUY_CARD action, which should contain the card to be bought.
    void handleCardBought(const QJsonObject &data);

    void handleGainedMoney(const QJsonObject &data);

    void handleReaction(const QJsonObject &data);
    
    void handleAllTreasuresPlayed();

    void handlePhaseChanged(const QJsonObject &data);
    void handleTurnEnded(const QJsonObject &data);
    void handleSelectionRequest(const QJsonObject &data);
    
    void updateGameState(const Network::GameStatePayload &payload);
    void startTurnWhenReady();
};