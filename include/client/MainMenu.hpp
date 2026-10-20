#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainMenu; }
QT_END_NAMESPACE

class MainMenu : public QWidget {
    Q_OBJECT

signals:
    void createGameClicked();
    void joinGameClicked();
    void leaderboardClicked();
    void quitClicked();

private slots:
    void onCreateGameClicked();
    void onJoinGameClicked();
    void onLeaderboardClicked();
    void onQuitClicked();

private:
    Ui::MainMenu *ui;

public:
    explicit MainMenu(QWidget *parent = nullptr);
    ~MainMenu() override;
};