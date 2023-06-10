#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <cstring>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <WS2tcpip.h>

#define BUFFER_SIZE 1024

#include <QObject>
#include <QThread>
#include <QtWidgets>
#include <QString>

class Client : public QThread
{
    Q_OBJECT

    void run() override;

public:
    explicit Client(QObject *parent = nullptr);

    // Methods
    void printMessage(const QString&);
    void sendMessage(const QString&);
    void connexionAuServeur();

    // Attributs
    QString adresseIPServeur{};
    int portServeur{};
    QString nomFichier{};

    //Affichage des messages
    QTextEdit* messagesClient;

signals:
    void processEnd(const bool &);

};

#endif // CLIENT_H
