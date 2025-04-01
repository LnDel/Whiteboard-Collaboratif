#ifndef SERVER_H
#define SERVER_H

#include "qcolor.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QMap>
#include <QVector>
#include <QLine>

// Structure pour stocker les informations sur les clients
struct ClientInfo {
    QTcpSocket *tcpSocket;
    QHostAddress udpAddress;
    quint16 udpPort;
    QColor color;
};

class WhiteboardServer : public QObject {
    Q_OBJECT

public:
    explicit WhiteboardServer(QObject *parent = nullptr);
    ~WhiteboardServer();

    void startServer(quint16 tcpPort, quint16 udpPort);

private slots:
    void newClientConnection();
    void handleClientMessage();
    void clientDisconnected();
    void handleUdpMessage();

private:
    QTcpServer *tcpServer;
    QUdpSocket *udpSocket;
    QMap<int, ClientInfo> clients;
    QVector<QLine> whiteboard;
    int nextClientId;

    void broadcastClientList();
    void broadcastUdpMessage(const QByteArray &message);
    void sendCurrentWhiteboardToClient(int clientId);
    void updateWhiteboardAndBroadcast(const QVector<QLine> &newDrawing);
};

#endif // SERVER_H
