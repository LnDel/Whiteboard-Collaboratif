#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QVector>
#include <QLine>
#include <QMouseEvent>
#include <QPainter>

class Client : public QWidget {
    Q_OBJECT

public:
    Client(QWidget *parent = nullptr);
    ~Client();
    void setEraseMode(bool erase);
    void clearCanvas();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void connectToServer();
    void handleTcpMessage();
    void handleUdpMessage();

private:
    bool drawing;
    bool eraseMode;
    bool mousePressed;
    int mouseX, mouseY;
    QPoint lastPoint;
    QVector<QLine> lines;

    QTcpSocket *tcpSocket;
    QUdpSocket *udpSocket;
    quint16 udpPort;
};

#endif // CLIENT_H
