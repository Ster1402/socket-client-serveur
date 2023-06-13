#include "client.h"


Client::Client(QObject *parent) : QThread(parent)
{

}

void Client::run()
{
    try {
        connexionAuServeur();
    }  catch (...) {
        qDebug() << "Erreur d'éxécution de la connexion Client";
    }
    emit processEnd(true);
}

void Client::printMessage(const QString& message)
{
    messagesClient->append("\n-------------------\n");
    messagesClient->append(message);
}

void Client::connexionAuServeur()
{
    if (adresseIPServeur.isEmpty())
    {
        printMessage(" Adresse Ip Manquante !");
        return;
    }

    if ( portServeur < 1000 || portServeur > 9999  )
    {
        printMessage(" Veuillez utilisez un port correcte ( 1000 < port < 9999 ) !");
        return;
    }

    if ( nomFichier.isEmpty())
    {
        printMessage(" Nom du fichier Manquant !");
        return;
    }

    std::string fileName = nomFichier.toStdString();

    // Initialisation de Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printMessage("Erreur d'initialisation de Winsock.");
        return;
    }

    // Création du socket UDP
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        printMessage("Erreur de création du socket.");
        WSACleanup();
        return;
    }

    // Configuration de l'adresse du serveur
    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portServeur);

    // Convertir l'adresse IP du serveur en format binaire
    if (inet_pton(AF_INET, adresseIPServeur.toStdString().c_str() , &(serverAddr.sin_addr)) <= 0)
    {
        printMessage("Erreur de conversion de l'adresse IP ou port de communication incorrecte.");
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    // Envoi du message "SYN" au serveur
    const char* synMessage = "SYN";
    if (sendto(clientSocket, synMessage, strlen(synMessage), 0, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        printMessage("Erreur d'envoi du message SYN.");
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    // Réception du message SYN-ACK du serveur
    int recvSize = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, NULL, NULL);
    if (recvSize == SOCKET_ERROR)
    {
        printMessage("Erreur lors de la réception du message SYN-ACK.");
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    // Traitement du message SYN-ACK
    std::string synAckMessage(buffer);
    if (synAckMessage.substr(0, 7) == "SYN-ACK")
    {
        printMessage("Message reçu du serveur : " + QString::fromStdString(synAckMessage));

        // Répondre avec un message "ACK" sur le port public
        const char* ackMessage = "ACK";
        if (sendto(clientSocket, ackMessage, strlen(ackMessage), 0, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            printMessage("Erreur d'envoi du message ACK.");
            closesocket(clientSocket);
            WSACleanup();
            return;
        }

        // Envoi du nom du fichier recherché au serveur
        if (sendto(clientSocket, fileName.c_str(), fileName.length(), 0, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            printMessage("Erreur d'envoi du nom de fichier.");
            closesocket(clientSocket);
            WSACleanup();
            return;
        }

        // Réception du message "FIN" du serveur
        QString finMessage = "FIN";

        while(true){
            int recvSize = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0,NULL, NULL);
            QString messageServeur = QString(buffer);
            if (recvSize == SOCKET_ERROR)
            {
                printMessage("Erreur de réception du message FIN.");
                break;
            }
            else if (messageServeur.startsWith(finMessage))
            {
                printMessage("Message FIN reçu du serveur !");
                break;
            }
            else
            {
                // Reception des données du serveur
                printMessage("Message de données reçu du serveur : " + messageServeur);
            }
        }

    }

    // Fermeture du socket et nettoyage
    closesocket(clientSocket);
    WSACleanup();
}
