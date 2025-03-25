#pragma once

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QVector>
#include <QLine>

class Client : public QWidget
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = nullptr);
    ~Client();

    void clearCanvas();
    void setEraseMode(bool erase);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QPoint lastPoint;
    QVector<QLine> lines;
    bool drawing;
    bool eraseMode;
    float mouseX;
    float mouseY;
    bool mousePressed;
    int penWidth;
};
