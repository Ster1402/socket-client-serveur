#include "serveur.h"

Serveur::Serveur(QObject* parent) : QThread(parent)
{

}

void Serveur::run()
{
    try {
        lancementServeur();
    }  catch (...) {
        qDebug() << "Erreur d'éxecution du serveur";
    }
    emit processEnd(true);
}

void Serveur::printMessage(const QString &message)
{
    messagesServeur->append("\n-------------------\n");
    messagesServeur->append(message);
}

//Version utilisant les classes optimiser de QtNetwork

void Serveur::lancementServeur()
{
    if (portServeur < 1000 || portServeur > 9999 )
    {
        printMessage("Veuillez utiliser un port valide (1000 < port < 9999) !");
        return;
    }

    // Création du socket du serveur
    QUdpSocket serverSocket;
    if (!serverSocket.bind(QHostAddress::Any, portServeur)) {
        printMessage("Erreur lors de la création du socket.");
        return;
    }

    QString ip = serverSocket.localAddress().toString();
    adresseIP->setText(ip);

    printMessage("Le serveur est prêt à recevoir des messages.");

    while (serverSocket.state() == QUdpSocket::BoundState) {
        serverSocket.waitForReadyRead();
        while (serverSocket.hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(serverSocket.pendingDatagramSize());
            QHostAddress senderAddress;
            quint16 senderPort;

            serverSocket.readDatagram(datagram.data(), datagram.size(), &senderAddress, &senderPort);

            // Traiter le datagramme reçu
            processMessage(senderAddress, senderPort, datagram, serverSocket);
        }
    }

    // Fermer le socket du serveur
    serverSocket.close();
}

void Serveur::processMessage(const QHostAddress& clientAddress, quint16 clientPort, const QByteArray& message,  QUdpSocket& serverSocket)
{
    QString synMessage = "SYN";
    QString ackMessage = "ACK";
    //QString finMessage = "FIN";

    if (message == synMessage.toUtf8()) {
        // Réception du message SYN
        printMessage("Message reçu du client : " + QString(message));

        QString synAckMessage = "SYN-ACK" + QString::number(portServeur);

        // Envoi du message SYN-ACK
        if (serverSocket.writeDatagram(synAckMessage.toUtf8(), clientAddress, clientPort) == -1) {
            printMessage("Erreur d'envoi du message SYN-ACK au client.");
        }
    }
    else if (message == ackMessage.toUtf8()) {
        // Réception du message ACK
        printMessage("Message reçu du client : " + QString(message));

        // Réception du nom de fichier recherché
        QByteArray fileName;
        if (serverSocket.hasPendingDatagrams()) {
            fileName.resize(serverSocket.pendingDatagramSize());
            serverSocket.readDatagram(fileName.data(), fileName.size());
        }

        printMessage("Nom de fichier reçu du client : " + QString(fileName));

        // Envoi de messages de données avec un numéro de séquence
        int numero = rand() % 15 + 2;

        for (int i = 0; i <= numero; i++) {
            QString sequenceNumber = QString::number(i).rightJustified(6, '0');
            QString dataMessage = sequenceNumber + "smsdata";

            printMessage("Message de données envoyés au client, " + dataMessage);

            if (serverSocket.writeDatagram(dataMessage.toUtf8(), clientAddress, clientPort) == -1) {
                printMessage("Erreur d'envoi des messages de données au client.");
            }
        }

        // Envoi du message de fin au client
        QByteArray finResponse = "FIN";
        if (serverSocket.writeDatagram(finResponse, clientAddress, clientPort) == -1) {
            printMessage("Erreur d'envoi du message FIN au client.");
        }

        printMessage("-- FIN au client. --");
    }
}

// Version qui utilise les sockets C++ natif SOCKET

//void Serveur::processMessage(SOCKET serverSocket, SOCKADDR_IN clientAddress, char* message) {
//    std::string synMessage = "SYN";
//    std::string ackMessage = "ACK";
//    std::string finMessage = "FIN";

//    if (strncmp(message, synMessage.c_str(), synMessage.length()) == 0) {
//        // Réception du message SYN
//        printMessage("Message reçu du client : " + QString(message));

//        std::string synAckMessage = "SYN-ACK" + std::to_string(portServeur);

//        // Envoi du message SYN-ACK
//        if (sendto(serverSocket, synAckMessage.c_str(), synAckMessage.length(), 0, (SOCKADDR*)&clientAddress, sizeof(clientAddress)) == SOCKET_ERROR) {
//            printMessage("Erreur d'envoi du message SYN-ACK au client.");
//        }
//    }
//    else if (strncmp(message, ackMessage.c_str(), ackMessage.length()) == 0) {
//        // Réception du message ACK
//        printMessage("Message reçu du client : " + QString(message));
//        // Réception du nom de fichier recherché
//        char fileName[BUFFER_SIZE];
//        int recvSize = recvfrom(serverSocket, fileName, BUFFER_SIZE, 0, NULL, NULL);
//        if (recvSize == SOCKET_ERROR) {
//            printMessage("Erreur lors de la réception du nom de fichier.");
//        }
//        else {
//            fileName[recvSize] = '\0';
//            printMessage("Nom de fichier reçu du client : " + QString(fileName));
//        }

//        // Envoi de messages de données avec un numéro de séquence
//        int numero = rand() % 10 + 1;

//        for (int i = 0; i <= numero; i++) {
//            std::string sequenceNumber = std::to_string(i);
//            sequenceNumber = std::string(6 - sequenceNumber.length(), '0') + sequenceNumber;
//            std::string dataMessage = sequenceNumber + "smsdata";

//            printMessage(" Data Message: " + QString::fromStdString(dataMessage));

//            if (sendto(serverSocket, dataMessage.c_str(), dataMessage.length(), 0, (SOCKADDR*)&clientAddress, sizeof(clientAddress)) == SOCKET_ERROR) {
//                printMessage("Erreur d'envoi des messages de données au client.");
//            }

//        }

//    }
//    else {

//        // Envoi du message de fin au client
//        std::string finResponse = "FIN";
//        if (sendto(serverSocket, finResponse.c_str(), finResponse.length(), 0, (SOCKADDR*)&clientAddress, sizeof(clientAddress)) == SOCKET_ERROR) {
//            printMessage("Erreur d'envoi du message FIN au client.");
//        }

//        // Fermeture de la connexion avec le client
//        closesocket(serverSocket);
//        WSACleanup();
//    }
//}

//void Serveur::lancementServeur()
//{
//    if (portServeur < 1000 || portServeur > 9999 )
//    {
//        printMessage(" Veuillez utilisez un port correcte ( 1000 < port < 9999 ) !");
//        return;
//    }

//    // Initialisation de Winsock
//    WSADATA wsaData;
//    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//        printMessage("Erreur d'initialisation de Winsock.");
//        return;
//    }

//    // Création du socket du serveur
//    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
//    if (serverSocket == INVALID_SOCKET) {
//        printMessage("Erreur de création du socket.");
//        WSACleanup();
//        return;
//    }

//    // Configuration de l'adresse du serveur
//    SOCKADDR_IN serverAddress;
//    serverAddress.sin_family = AF_INET;
//    serverAddress.sin_addr.s_addr = INADDR_ANY;//inet_addr("127.0.0.1");
//    serverAddress.sin_port = htons(portServeur);

//    // Lier le socket à l'adresse du serveur
//    if (bind(serverSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
//        printMessage("Erreur d'association du socket à l'adresse du serveur.");
//        closesocket(serverSocket);
//        WSACleanup();
//        return;
//    }

//    printMessage("Le serveur est prêt à recevoir des messages. ");

//    char buffer[BUFFER_SIZE];
//    SOCKADDR_IN clientAddress;
//    int clientAddressSize = sizeof(clientAddress);

//    while (true) {
//        // Attendre la réception d'un datagramme du client
//        memset(buffer, 0, BUFFER_SIZE);
//        int recvSize = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, (SOCKADDR*)&clientAddress, &clientAddressSize);
//        if (recvSize == SOCKET_ERROR) {
//            printMessage("Erreur lors de la réception du datagramme.");
//            closesocket(serverSocket);
//            WSACleanup();
//            return;
//        }

//        // Traiter le datagramme reçu
//        processMessage(serverSocket, clientAddress, buffer);
//    }

//    // Fermer le socket du serveur
//    closesocket(serverSocket);

//    // Nettoyage de Winsock
//    WSACleanup();

//}
