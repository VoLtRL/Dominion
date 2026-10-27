#pragma once

#include <QWidget>
#include <QString>
#include <QtGlobal>

class SettingsDialog;

QT_BEGIN_NAMESPACE
namespace Ui { class MainMenu; }
QT_END_NAMESPACE

class MainMenu : public QWidget {
    Q_OBJECT

signals:
    void createGameClicked();
    void joinRoomClicked();
    void leaderboardClicked();
    void quitClicked();
    void settingsChanged(const QString &playerName, const QString &hostAddress, quint16 hostPort);

private slots:
    void onCreateGameClicked();
    void onJoinRoomClicked();
    void onLeaderboardClicked();
    void onQuitClicked();
    void onSettingsButtonClicked();

private:
    Ui::MainMenu *ui;
    SettingsDialog *settingsDialog;

public:
    explicit MainMenu(QWidget *parent = nullptr);
    ~MainMenu() override;
    void setSettings(const QString &playerName, const QString &hostAddress, quint16 hostPort);
};