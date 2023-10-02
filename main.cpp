#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <poll.h>
#define MAX_CLIENTS 100
#define PORT_NUMBER 8085

int ServerSocketCreation();
void reusePort(int serverSocket);
sockaddr_in6 &binding(int serverSocket, sockaddr_in6 &serverAddr);
void listening(int serverSocket);

int main() {
	int serverSocket;
	int maxSocket, activity;
	fd_set readfds;
	struct sockaddr_in6 serverAddr;

	serverSocket = ServerSocketCreation();
	reusePort(serverSocket);

	// Configuration de l'adresse du serveur pour IPv6
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin6_family = AF_INET6;
	serverAddr.sin6_port = htons(PORT_NUMBER);
	serverAddr.sin6_addr = in6addr_any; // Écoute sur toutes les interfaces IPv4 et IPv6

//	serverAddr.sin_family = AF_INET6;
//	serverAddr.sin_port = htons(PORT_NUMBER);
//	serverAddr.sin_addr.s_addr = INADDR_ANY; // Accepter les connexions de n'importe quelle adresse IP
	
	serverAddr = binding(serverSocket, serverAddr);
	listening(serverSocket);

	std::cout << "Attente de connexions sur le port "<< PORT_NUMBER <<"..." << std::endl;

	// Variables pour stocker les données lues
	char buffer[1024];

	struct pollfd clients[MAX_CLIENTS];

	int listenSocket = serverSocket/* initialisation du socket d'écoute */;
	int clientCount = 0; // Compteur de clients connectés

	// Initialisez la structure pollfd pour le socket d'écoute
	clients[0].fd = listenSocket;
	clients[0].events = POLLIN; // Surveiller les nouvelles connexions entrantes

	while (1) {
		int ready = poll(clients, clientCount + 1, -1);
		if (ready == -1) {
			perror("Erreur lors de l'appel à poll");
			break;
		}

		// Parcourez les structures pollfd pour vérifier les événements sur chaque socket
		for (int i = 0; i <= clientCount; i++) {
			if (clients[i].revents & POLLIN) {
				if (i == 0) {
					// Le socket d'écoute est prêt à accepter une nouvelle connexion
					int newSocket = accept(listenSocket, NULL, NULL);
					if (newSocket == -1) {
						perror("Erreur lors de l'acceptation de la nouvelle connexion");
					} else {
						// Ajoutez le nouveau socket à la liste des sockets surveillés
						std::cout << "Client connected" << std::endl;
						clients[clientCount + 1].fd = newSocket;
						clients[clientCount + 1].events = POLLIN;
						clientCount++;
					}
				} else {
					// C'est un socket client existant, lisez les données et répondez
					// Ici, vous pouvez gérer la lecture des données depuis le socket client
					// et traiter les demandes HTTP
				}
			}
		}
	}

	close(serverSocket);
	return 0;
}

void listening(int serverSocket) {
	if (listen(serverSocket, SOMAXCONN) < 0) {
		perror("Erreur lors de la mise en écoute");
		exit(EXIT_FAILURE);
	}
}
sockaddr_in6 &binding(int serverSocket, sockaddr_in6 &serverAddr) {
	if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
		perror("Erreur lors de la liaison du socket");
		exit(EXIT_FAILURE);
	}
	return serverAddr;
}
void reusePort(int serverSocket)
{
	int reuse = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
		perror("setsockopt");
		exit(1);
	}
}
int ServerSocketCreation() {
	int serverSocket;
	if ((serverSocket = socket(AF_INET6, SOCK_STREAM, 0)) == 0) {
		perror("Erreur lors de la création du socket");
		exit(EXIT_FAILURE);
	}
	return serverSocket;
}
