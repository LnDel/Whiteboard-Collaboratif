#include "server.h"
#include <QDataStream>
#include <QColor>
#include <QDebug>

WhiteboardServer::WhiteboardServer(QObject *parent)
    : QObject(parent), nextClientId(1) {
    tcpServer = new QTcpServer(this);
    udpSocket = new QUdpSocket(this);
    qDebug() << "Serveur initialisé.";
}

WhiteboardServer::~WhiteboardServer() {
    delete tcpServer;
    delete udpSocket;
    qDebug() << "Serveur arrêté.";
}

void WhiteboardServer::startServer(quint16 tcpPort, quint16 udpPort) {
    if (!tcpServer->listen(QHostAddress::Any, tcpPort)) {
        qCritical() << "Impossible de démarrer le serveur TCP";
        return;
    }

    connect(tcpServer, &QTcpServer::newConnection, this, &WhiteboardServer::newClientConnection);

    if (!udpSocket->bind(QHostAddress::Any, udpPort)) {
        qCritical() << "Impossible de lier le serveur UDP";
        return;
    }

    connect(udpSocket, &QUdpSocket::readyRead, this, &WhiteboardServer::handleUdpMessage);

    qDebug() << "Serveur démarré sur TCP:" << tcpPort << "et UDP:" << udpPort;
}

void WhiteboardServer::newClientConnection() {
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
    if (!clientSocket) return;

    int clientId = nextClientId++;
    ClientInfo clientInfo;
    clientInfo.tcpSocket = clientSocket;
    clientInfo.color = QColor::fromHsv(clientId * 40 % 360, 255, 255); // Attribuer une couleur unique

    clients[clientId] = clientInfo;

    connect(clientSocket, &QTcpSocket::readyRead, this, &WhiteboardServer::handleClientMessage);
    connect(clientSocket, &QTcpSocket::disconnected, this, &WhiteboardServer::clientDisconnected);

    qDebug() << "Nouveau client connecté, ID:" << clientId;

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << clientId << clientInfo.color;
    clientSocket->write(data);

    sendCurrentWhiteboardToClient(clientId);

    broadcastClientList();
}

void WhiteboardServer::handleClientMessage() {
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket) return;

    int clientId = -1;
    for (auto id : clients.keys()) {
        if (clients[id].tcpSocket == clientSocket) {
            clientId = id;
            break;
        }
    }
    if (clientId == -1) return;

    QDataStream in(clientSocket);
    QString messageType;
    in >> messageType;

    qDebug() << "Message TCP reçu de client" << clientId << ":" << messageType;

    if (messageType == "SYNC") {
        QString udpAddress;
        quint16 udpPort;
        in >> udpAddress >> udpPort;

        clients[clientId].udpAddress = QHostAddress(udpAddress);
        clients[clientId].udpPort = udpPort;

        qDebug() << "Client" << clientId << "synchronisé sur UDP avec adresse" << udpAddress << "et port" << udpPort;
    }
    else if (messageType == "DRAW") {
        QVector<QLine> newDrawing;
        in >> newDrawing;

        qDebug() << "Nouveau dessin reçu de client" << clientId << "avec" << newDrawing.size() << "lignes.";

        updateWhiteboardAndBroadcast(newDrawing);
    }
}

void WhiteboardServer::clientDisconnected() {
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket) return;

    int clientId = -1;
    for (auto id : clients.keys()) {
        if (clients[id].tcpSocket == clientSocket) {
            clientId = id;
            break;
        }
    }
    if (clientId == -1) return;

    clients.remove(clientId);
    qDebug() << "Client" << clientId << "déconnecté";

    broadcastClientList();
}

void WhiteboardServer::broadcastClientList() {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << QString("CLIENT_LIST") << clients.keys();

    for (auto &client : clients) {
        client.tcpSocket->write(data);
    }
    qDebug() << "Liste des clients diffusée.";
}

void WhiteboardServer::handleUdpMessage() {
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        QHostAddress senderAddr;
        quint16 senderPort;

        udpSocket->readDatagram(datagram.data(), datagram.size(), &senderAddr, &senderPort);

        int senderId = -1;
        for (auto id : clients.keys()) {
            if (clients[id].udpAddress == senderAddr && clients[id].udpPort == senderPort) {
                senderId = id;
                break;
            }
        }
        if (senderId == -1) continue;

        qDebug() << "Message UDP reçu de client" << senderId;

        broadcastUdpMessage(datagram);
    }
}

void WhiteboardServer::broadcastUdpMessage(const QByteArray &message) {
    for (auto &client : clients) {
        if (client.udpAddress.isNull() || client.tcpSocket->state() != QAbstractSocket::ConnectedState)
            continue;

        udpSocket->writeDatagram(message, client.udpAddress, client.udpPort);
    }
    qDebug() << "Message UDP diffusé à tous les clients.";
}

void WhiteboardServer::sendCurrentWhiteboardToClient(int clientId) {
    qDebug() << "Envoi du tableau blanc actuel au client" << clientId;

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << QString("WHITEBOARD") << whiteboard;

    clients[clientId].tcpSocket->write(data);
}

void WhiteboardServer::updateWhiteboardAndBroadcast(const QVector<QLine> &newDrawing) {
    whiteboard.append(newDrawing);

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << QString("DRAW") << newDrawing;

    broadcastUdpMessage(data);
}
