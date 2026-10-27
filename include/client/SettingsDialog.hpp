#pragma once

#include <QDialog>
#include <QString>
#include <QtGlobal>

QT_BEGIN_NAMESPACE
namespace Ui { class SettingsDialog; }
QT_END_NAMESPACE

class SettingsDialog : public QDialog {
    Q_OBJECT
private:
    Ui::SettingsDialog *ui;

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog() override;
    void setValues(const QString &playerName, const QString &hostAddress, quint16 hostPort);

signals:
    void settingsChanged(const QString &playerName, const QString &hostAddress, quint16 hostPort);

private slots:
    void onCancelClicked();
    void onSaveClicked();
};