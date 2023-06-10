#include "serveur.h"

Serveur::Serveur(QObject* parent) : QThread(parent)
{

}

void Serveur::run()
{
    try {
        lancementServeur();
    }  catch (...) {
        qDebug("Erreur d'éxecution du serveur");
    }
    emit processEnd(true);
}

void Serveur::processMessage(SOCKET serverSocket, SOCKADDR_IN clientAddress, char* message) {
    std::string synMessage = "SYN";
    std::string ackMessage = "ACK";
    std::string finMessage = "FIN";

    // Génération de nombre aléatoire
    srand(time(NULL));

    if (strncmp(message, synMessage.c_str(), synMessage.length()) == 0) {
        // Réception du message SYN
        printMessage("Message reçu du client : " + QString(message));

        std::string synAckMessage = "SYN-ACK" + std::to_string(portServeur);

        // Envoi du message SYN-ACK
        if (sendto(serverSocket, synAckMessage.c_str(), synAckMessage.length(), 0, (SOCKADDR*)&clientAddress, sizeof(clientAddress)) == SOCKET_ERROR) {
            printMessage("Erreur d'envoi du message SYN-ACK au client.");
        }
    }
    else if (strncmp(message, ackMessage.c_str(), ackMessage.length()) == 0) {
        // Réception du message ACK
        printMessage("Message reçu du client : " + QString(message));
        // Réception du nom de fichier recherché
        char fileName[BUFFER_SIZE];
        int recvSize = recvfrom(serverSocket, fileName, BUFFER_SIZE, 0, NULL, NULL);
        if (recvSize == SOCKET_ERROR) {
            printMessage("Erreur lors de la réception du nom de fichier.");
        }
        else {
            fileName[recvSize] = '\0';
            printMessage("Nom de fichier reçu du client : " + QString(fileName));
        }

        // Envoi de messages de données avec un numéro de séquence
        int numero = rand() % 10 + 1;

        for (int i = 0; i <= numero; i++) {
            std::string sequenceNumber = std::to_string(i);
            sequenceNumber = std::string(6 - sequenceNumber.length(), '0') + sequenceNumber;
            std::string dataMessage = sequenceNumber + "smsdata";

            printMessage(" Data Message: " + QString::fromStdString(dataMessage));

            if (sendto(serverSocket, dataMessage.c_str(), dataMessage.length(), 0, (SOCKADDR*)&clientAddress, sizeof(clientAddress)) == SOCKET_ERROR) {
                printMessage("Erreur d'envoi des messages de données au client.");
            }

        }

    }
    else {

        // Envoi du message de fin au client
        std::string finResponse = "FIN";
        if (sendto(serverSocket, finResponse.c_str(), finResponse.length(), 0, (SOCKADDR*)&clientAddress, sizeof(clientAddress)) == SOCKET_ERROR) {
            printMessage("Erreur d'envoi du message FIN au client.");
        }

        // Fermeture de la connexion avec le client
        closesocket(serverSocket);
        WSACleanup();
    }
}

void Serveur::printMessage(const QString &message)
{
    messagesServeur->append("\n-------------------\n");
    messagesServeur->append(message);
}

void Serveur::lancementServeur()
{
    if (portServeur < 1000 || portServeur > 9999 )
    {
        printMessage(" Veuillez utilisez un port correcte ( 1000 < port < 9999 ) !");
        return;
    }

    // Initialisation de Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printMessage("Erreur d'initialisation de Winsock.");
        return;
    }

    // Création du socket du serveur
    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printMessage("Erreur de création du socket.");
        WSACleanup();
        return;
    }

    // Configuration de l'adresse du serveur
    SOCKADDR_IN serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(portServeur);

    // Lier le socket à l'adresse du serveur
    if (bind(serverSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        printMessage("Erreur d'association du socket à l'adresse du serveur.");
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    printMessage("Le serveur est prêt à recevoir des messages. ");

    char buffer[BUFFER_SIZE];
    SOCKADDR_IN clientAddress;
    int clientAddressSize = sizeof(clientAddress);

    while (true) {
        // Attendre la réception d'un datagramme du client
        memset(buffer, 0, BUFFER_SIZE);
        int recvSize = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, (SOCKADDR*)&clientAddress, &clientAddressSize);
        if (recvSize == SOCKET_ERROR) {
            printMessage("Erreur lors de la réception du datagramme.");
            closesocket(serverSocket);
            WSACleanup();
            return;
        }

        // Traiter le datagramme reçu
        processMessage(serverSocket, clientAddress, buffer);
    }

    // Fermer le socket du serveur
    closesocket(serverSocket);

    // Nettoyage de Winsock
    WSACleanup();

}
