#include "mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), clientWidget(new Client(this)) {
    setWindowTitle("Application de Dessin");
    setCentralWidget(clientWidget);
    resize(800, 600);

    QVBoxLayout *layout = new QVBoxLayout();

    eraseButton = new QPushButton("Mode Gomme", this);
    connect(eraseButton, &QPushButton::clicked, this, &MainWindow::toggleEraseMode);
    layout->addWidget(eraseButton);

    clearButton = new QPushButton("Effacer Tout", this);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearCanvas);
    layout->addWidget(clearButton);

    QWidget *buttonWidget = new QWidget(this);
    buttonWidget->setLayout(layout);
    setMenuWidget(buttonWidget);
}

MainWindow::~MainWindow() {
    delete clientWidget;
}

void MainWindow::toggleEraseMode() {
    bool erase = eraseButton->text() == "Mode Gomme";
    clientWidget->setEraseMode(erase);
    eraseButton->setText(erase ? "Mode Dessin" : "Mode Gomme");
}

void MainWindow::clearCanvas() {
    clientWidget->clearCanvas();
}
