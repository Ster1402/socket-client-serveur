#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , client(new Client(this))
    , serveur(new Serveur(this))
{
    ui->setupUi(this);

    client->messagesClient = ui->messagesClient;
    serveur->adresseIP = ui->adresseIPServeur;
    serveur->messagesServeur = ui->messagesServeur;


    // Enregistrement des slots
    QObject::connect(ui->lancerServeur, &QPushButton::pressed,
                     this, [&](){ this->launchServer(ui->lancerServeur); });

    QObject::connect(ui->connexionServeur, &QPushButton::pressed,
                     this, [&](){ this->connectToServer(ui->connexionServeur); });

    QObject::connect(serveur, &Serveur::processEnd, ui->lancerServeur, [&](){ ui->lancerServeur->setDisabled(false); });
    QObject::connect(client, &Client::processEnd, ui->connexionServeur, [&](){ ui->connexionServeur->setDisabled(false); });
}

MainWindow::~MainWindow()
{
    serveur->quit();
    serveur->wait();

    client->quit();
    client->wait();
    delete ui;
}

void MainWindow::launchServer(QObject *sender)
{
    QPushButton* btnSender = (QPushButton*) sender;
    btnSender->setDisabled(true);
    serveur->portServeur = ui->portServeur->text().toInt();
    serveur->start(QThread::HighestPriority);
}

void MainWindow::connectToServer(QObject *sender)
{
    QPushButton* btnSender = (QPushButton*) sender;
    btnSender->setDisabled(true);
    client->adresseIPServeur = ui->adresseIPClient->text();
    client->portServeur = ui->portClient->text().toInt();
    client->nomFichier = ui->fichierClient->text();

    client->start(QThread::HighPriority);
}
