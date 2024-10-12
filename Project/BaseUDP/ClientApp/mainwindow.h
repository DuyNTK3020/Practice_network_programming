#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_connectButton_clicked();
    void on_sendButton_clicked();
    void readPendingDatagrams();

private:
    Ui::MainWindow *ui;
    QUdpSocket *udpSocket;
    bool isConnected;
    QHostAddress serverAddress;
    quint16 serverPort;
};
#endif // MAINWINDOW_H
