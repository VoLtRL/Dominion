#pragma once
#include <QMainWindow>
#include <QTimer>
#include <QJsonObject>

#include "client/network/NetworkManager.hpp"
#include "shared/gameplay/Game.hpp"

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
    std::optional<Game> instance;

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
    void connectNavigation();
    void connectNetwork();
    void showPage(Page page);
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
};