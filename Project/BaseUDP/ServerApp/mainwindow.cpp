#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>
#include <QMap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , udpSocket(new QUdpSocket(this))
    , isServerRunning(false)
{
    ui->setupUi(this);
    connect(udpSocket, &QUdpSocket::readyRead, this, &MainWindow::readPendingDatagrams);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startServerButton_clicked()
{
    if (!isServerRunning) {
        quint16 port = ui->portInput->text().toUShort();
        if (udpSocket->bind(QHostAddress::Any, port)) {
            ui->logTextEdit->append(QString("Server started on port %1").arg(port));
            isServerRunning = true;
        } else {
            ui->logTextEdit->append("Failed to start server.");
        }
    } else {
        udpSocket->close();
        ui->logTextEdit->append("Server stopped.");
        isServerRunning = false;
    }
}

// Bước 2: Lưu trữ thông tin client
QMap<QString, QPair<QHostAddress, quint16>> clients;

void MainWindow::readPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        QHostAddress sender;
        quint16 senderPort;
        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        // Tạo một key cho client dựa trên địa chỉ IP và cổng
        QString clientKey = QString("%1:%2").arg(sender.toString()).arg(senderPort);

        // Lưu thông tin client
        if (!clients.contains(clientKey)) {
            clients.insert(clientKey, qMakePair(sender, senderPort));
            ui->logTextEdit->append(QString("New client connected: %1").arg(clientKey));
        }

        // Xử lý và phản hồi thông điệp từ client
        QString message = QString("Received from %1:%2 - %3")
                              .arg(sender.toString())
                              .arg(senderPort)
                              .arg(QString(datagram));

        ui->logTextEdit->append(message);

        // Echo lại message về client
        udpSocket->writeDatagram(datagram, sender, senderPort);
    }
}


