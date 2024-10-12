#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , udpSocket(new QUdpSocket(this))
    , isConnected(false)
{
    ui->setupUi(this);
    connect(udpSocket, &QUdpSocket::readyRead, this, &MainWindow::readPendingDatagrams);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connectButton_clicked()
{
    if (!isConnected) {
        serverAddress = QHostAddress(ui->ipInput->text());
        serverPort = ui->portInput->text().toUShort();

        // Bind client to any available address and port 0 for dynamic port assignment
        udpSocket->bind(QHostAddress::Any, 0);

        // In ra cổng mà client bind
        ui->logTextEdit->append(QString("Client bound to port %1").arg(udpSocket->localPort()));
        ui->logTextEdit->append(QString("Connected to server at %1:%2")
                                    .arg(serverAddress.toString())
                                    .arg(serverPort));

        isConnected = true;
    }
}

void MainWindow::on_sendButton_clicked()
{
    if (isConnected) {
        QString message = ui->messageInput->text();
        QByteArray data = message.toUtf8();

        // Gửi dữ liệu đến địa chỉ server
        udpSocket->writeDatagram(data, serverAddress, serverPort);
        ui->logTextEdit->append("Sent: " + message);

        qDebug() << "Sent: " << message << " to " << serverAddress.toString() << ":" << serverPort;
    }
}

void MainWindow::readPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        QHostAddress sender;
        quint16 senderPort;
        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        QString message = QString("Received from server - %1").arg(QString(datagram));
        ui->logTextEdit->append(message);

        // Debug thông tin gói tin nhận được
        qDebug() << "Received datagram from: " << sender.toString() << ":" << senderPort;
        qDebug() << "Data: " << QString(datagram);
    }
}
