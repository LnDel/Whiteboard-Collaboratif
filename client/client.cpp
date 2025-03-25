#include "client.h"
#include <QLineF>

Client::Client(QWidget *parent) : QWidget(parent), drawing(false), eraseMode(false), mousePressed(false)
{
    setWindowTitle("Client - Dessin à la souris");
    setFixedSize(800, 600);
    setStyleSheet("background-color: white;");
}

Client::~Client() {}

void Client::mousePressEvent(QMouseEvent *event)
{
    mouseX = event->pos().x();
    mouseY = event->pos().y();
    mousePressed = true;

    if (event->button() == Qt::LeftButton)
    {
        lastPoint = event->pos();
        if (eraseMode)
        {
            for (int i = lines.size() - 1; i >= 0; --i)
            {
                QLineF line(lastPoint, lines[i].p1());
                QLineF line2(lastPoint, lines[i].p2());

                if (line.length() < 10 || line2.length() < 10)
                {
                    lines.removeAt(i);
                }
            }
        }
        else
        {
            drawing = true;
        }
    }
    update();
}

void Client::mouseMoveEvent(QMouseEvent *event)
{
    mouseX = event->pos().x();
    mouseY = event->pos().y();

    if (drawing && (event->buttons() & Qt::LeftButton) && !eraseMode)
    {
        QLine newLine(lastPoint, event->pos());
        lines.append(newLine);
        lastPoint = event->pos();
        update();
    }
    else if (eraseMode && (event->buttons() & Qt::LeftButton))
    {
        // Parcours de la liste à l'envers
        for (int i = lines.size() - 1; i >= 0; --i)
        {
            QLineF line(event->pos(), lines[i].p1());
            QLineF line2(event->pos(), lines[i].p2());

            if (line.length() < 10 || line2.length() < 10)
            {
                lines.removeAt(i);
            }
        }
        update();
    }
}


void Client::mouseReleaseEvent(QMouseEvent *event)
{
    mousePressed = false;
    if (event->button() == Qt::LeftButton)
    {
        drawing = false;
    }
    update();
}

void Client::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPen pen(Qt::black, 2);
    painter.setPen(pen);

    painter.fillRect(rect(), Qt::white);

    if (eraseMode && mousePressed) {
        QRectF rectangle(mouseX-10, mouseY-10, 20.0, 20.0);
        painter.drawEllipse(rectangle);
    }

    // Dessiner les lignes
    for (const QLine &line : qAsConst(lines))
    {
        painter.drawLine(line);
    }
}

void Client::clearCanvas()
{
    lines.clear();
    update();
}

void Client::setEraseMode(bool erase)
{
    eraseMode = erase;
}
