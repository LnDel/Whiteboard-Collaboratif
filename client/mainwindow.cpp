#include "mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), clientWidget(new Client(this))
{
    setWindowTitle("Application de Dessin");
    setCentralWidget(clientWidget);
    resize(800, 600);

    // Créer un layout vertical pour les boutons
    QVBoxLayout *layout = new QVBoxLayout();

    // Créer le bouton Gomme
    eraseButton = new QPushButton("Mode Gomme", this);
    connect(eraseButton, &QPushButton::clicked, this, &MainWindow::toggleEraseMode);
    layout->addWidget(eraseButton);

    // Créer le bouton Effacer
    clearButton = new QPushButton("Effacer Tout", this);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearCanvas);
    layout->addWidget(clearButton);

    // Ajouter les boutons à la fenêtre
    QWidget *buttonWidget = new QWidget(this);
    buttonWidget->setLayout(layout);
    setMenuWidget(buttonWidget);
}

MainWindow::~MainWindow()
{
    delete clientWidget;
}

void MainWindow::toggleEraseMode()
{
    // Basculer entre les modes Gomme et Dessin
    bool erase = eraseButton->text() == "Mode Gomme";
    clientWidget->setEraseMode(erase);

    // Mettre à jour le texte du bouton en fonction du mode
    eraseButton->setText(erase ? "Mode Dessin" : "Mode Gomme");
}

void MainWindow::clearCanvas()
{
    clientWidget->clearCanvas();
}
