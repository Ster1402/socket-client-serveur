#ifndef SERVEUR_H
#define SERVEUR_H

#include <iostream>
#include <cstring>
#include <ctime>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <QUdpSocket>
#include <QHostAddress>


#define SERVER_PORT 12345
#define BUFFER_SIZE 1024


#include <QObject>
#include <QThread>
#include <QtWidgets>
#include <QString>

class Serveur: public QThread
{
    Q_OBJECT
    void run() override;

public:
    explicit Serveur(QObject *parent = nullptr);
    void printMessage(const QString& message);
    void lancementServeur();
//    void processMessage(SOCKET serverSocket, SOCKADDR_IN clientAddress, char* message);
    void processMessage(const QHostAddress& senderAddress, quint16 senderPort, const QByteArray& message, QUdpSocket &serverSocket);

    // Attributs
    int portServeur{};
    QString adresseIPServeur{};

    // Affichage des messages
    QTextEdit* messagesServeur;
    QLabel* adresseIP;

signals:
    void processEnd(const bool&);
};

#endif // SERVEUR_H
