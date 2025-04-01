#include "client.h"
#include <QDataStream>
#include <QHostAddress>
#include <QDebug>

Client::Client(QWidget *parent)
    : QWidget(parent), drawing(false), eraseMode(false), mousePressed(false), tcpSocket(new QTcpSocket(this)), udpSocket(new QUdpSocket(this)) {
    setWindowTitle("Client - Dessin à la souris");
    setFixedSize(800, 600);
    setStyleSheet("background-color: white;");

    connect(tcpSocket, &QTcpSocket::readyRead, this, &Client::handleTcpMessage);
    connect(udpSocket, &QUdpSocket::readyRead, this, &Client::handleUdpMessage);

    connectToServer();
}

Client::~Client() {
    delete tcpSocket;
    delete udpSocket;
}

void Client::connectToServer() {
    tcpSocket->connectToHost("127.0.0.1", 1234); // Utilisez le port TCP 1234
    if (tcpSocket->waitForConnected(3000)) {
        qDebug() << "Connecté au serveur TCP";

        // Envoyer un message de synchronisation UDP
        udpPort = 49153; // Utilisez le port UDP 49153
        if (udpSocket->bind(QHostAddress::AnyIPv4, udpPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
            qDebug() << "Socket UDP lié au port" << udpPort;

            QByteArray syncMessage;
            QDataStream out(&syncMessage, QIODevice::WriteOnly);
            out << QString("SYNC") << QHostAddress(QHostAddress::Any).toString() << udpPort;
            tcpSocket->write(syncMessage);

            qDebug() << "Message de synchronisation UDP envoyé au serveur.";
        } else {
            qCritical() << "Erreur de liaison du socket UDP au port" << udpPort << udpSocket->errorString();
        }
    } else {
        qCritical() << "Erreur de connexion au serveur TCP";
    }
}


void Client::handleTcpMessage() {
    QDataStream in(tcpSocket);
    QString messageType;
    in >> messageType;

    qDebug() << "Message TCP reçu :" << messageType;

    if (messageType == "WHITEBOARD") {
        QVector<QLine> receivedLines;
        in >> receivedLines;
        lines.append(receivedLines);
        qDebug() << "Tableau blanc reçu avec" << receivedLines.size() << "lignes.";
        update();
    } else if (messageType == "CLIENT_LIST") {
        // Gérer la liste des clients si nécessaire
    }
}

void Client::handleUdpMessage() {
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());

        QDataStream in(datagram);
        QString messageType;
        in >> messageType;

        qDebug() << "Message UDP reçu :" << messageType;

        if (messageType == "DRAW") {
            QVector<QLine> newDrawing;
            in >> newDrawing;
            lines.append(newDrawing);
            qDebug() << "Nouveau dessin reçu avec" << newDrawing.size() << "lignes.";
            update();
        }
    }
}

void Client::mousePressEvent(QMouseEvent *event) {
    mouseX = event->pos().x();
    mouseY = event->pos().y();
    mousePressed = true;

    if (event->button() == Qt::LeftButton) {
        lastPoint = event->pos();
        if (eraseMode) {
            for (int i = lines.size() - 1; i >= 0; --i) {
                QLineF line(lastPoint, lines[i].p1());
                QLineF line2(lastPoint, lines[i].p2());

                if (line.length() < 10 || line2.length() < 10) {
                    lines.removeAt(i);
                }
            }
        } else {
            drawing = true;
        }
    }
    update();
}

void Client::mouseMoveEvent(QMouseEvent *event) {
    mouseX = event->pos().x();
    mouseY = event->pos().y();

    if (drawing && (event->buttons() & Qt::LeftButton) && !eraseMode) {
        QLine newLine(lastPoint, event->pos());
        lines.append(newLine);
        lastPoint = event->pos();

        // Envoyer le nouveau dessin au serveur
        QByteArray drawMessage;
        QDataStream out(&drawMessage, QIODevice::WriteOnly);
        out << QString("DRAW") << QVector<QLine>{newLine};
        udpSocket->writeDatagram(drawMessage, QHostAddress("127.0.0.1"), 49153); // Utilisez le port UDP 49153

        qDebug() << "Nouveau dessin envoyé au serveur.";

        update();
    } else if (eraseMode && (event->buttons() & Qt::LeftButton)) {
        for (int i = lines.size() - 1; i >= 0; --i) {
            QLineF line(event->pos(), lines[i].p1());
            QLineF line2(event->pos(), lines[i].p2());

            if (line.length() < 10 || line2.length() < 10) {
                lines.removeAt(i);
            }
        }
        qDebug() << "Lignes effacées.";
        update();
    }
}

void Client::mouseReleaseEvent(QMouseEvent *event) {
    mousePressed = false;
    if (event->button() == Qt::LeftButton) {
        drawing = false;
    }
    update();
}

void Client::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QPen pen(Qt::black, 2);
    painter.setPen(pen);

    painter.fillRect(rect(), Qt::white);

    if (eraseMode && mousePressed) {
        QRectF rectangle(mouseX-10, mouseY-10, 20.0, 20.0);
        painter.drawEllipse(rectangle);
    }

    for (const QLine &line : qAsConst(lines)) {
        painter.drawLine(line);
    }
}

void Client::clearCanvas() {
    lines.clear();
    qDebug() << "Canvas effacé.";
    update();
}

void Client::setEraseMode(bool erase) {
    eraseMode = erase;
    qDebug() << "Mode gomme activé :" << erase;
}
