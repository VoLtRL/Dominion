#include "client/SettingsDialog.hpp"
#include "ui_SettingsDialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::SettingsDialog) {
    ui->setupUi(this);
    ui->PlayerNameEdit->setPlaceholderText("Player name");
    ui->HostAddressEdit->setText("localhost");
    ui->HostPortSpinBox->setRange(1, 65535);
    ui->HostPortSpinBox->setValue(4040);
    
    connect(ui->CancelButton, &QPushButton::clicked, this, &SettingsDialog::onCancelClicked);
    connect(ui->SaveButton, &QPushButton::clicked, this, &SettingsDialog::onSaveClicked);
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}


void SettingsDialog::setValues(const QString &playerName, const QString &hostAddress, quint16 hostPort) {
    ui->PlayerNameEdit->setText(playerName);
    ui->HostAddressEdit->setText(hostAddress);
    ui->HostPortSpinBox->setValue(hostPort);
}

void SettingsDialog::onCancelClicked() {
    reject();
}

void SettingsDialog::onSaveClicked() {
    QString playerName = ui->PlayerNameEdit->text();
    QString hostAddress = ui->HostAddressEdit->text();
    quint16 hostPort = ui->HostPortSpinBox->value();
    emit settingsChanged(playerName, hostAddress, hostPort);
    accept();
}