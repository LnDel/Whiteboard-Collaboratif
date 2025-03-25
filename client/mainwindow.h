#pragma once

#include <QMainWindow>
#include <QPushButton>
#include "client.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void toggleEraseMode();
    void clearCanvas();

private:
    Client *clientWidget;
    QPushButton *eraseButton;
    QPushButton *clearButton;
};
