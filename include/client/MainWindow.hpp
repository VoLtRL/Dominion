#pragma once
#include <QMainWindow>
#include <QThread>

#include "client/network/NetworkManager.hpp"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    Ui::MainWindow *ui; 
    NetworkManager *networkManager;

    QString playerName;

public:
    explicit MainWindow(QWidget *parent = nullptr, NetworkManager *networkManager = nullptr);
    
    ~MainWindow() override;

};