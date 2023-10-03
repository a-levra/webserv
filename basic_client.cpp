#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
	const char* server_ip = "127.0.0.1"; // Adresse IP de localhost
	int server_port = 5000;             // Port à utiliser
	const char* message = "Bonjour, serveur!"; // Message à envoyer

	// Création d'une socket
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1) {
		std::cerr << "Erreur lors de la création de la socket." << std::endl;
		return 1;
	}

	// Configuration de l'adresse du serveur
	sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(server_port);
	inet_pton(AF_INET, server_ip, &server_address.sin_addr);

	// Connexion au serveur
	if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
		std::cerr << "Erreur lors de la connexion au serveur." << std::endl;
		close(client_socket);
		return 1;
	}

	// Envoi du message au serveur
	ssize_t bytes_sent = send(client_socket, message, strlen(message), 0);
	if (bytes_sent == -1) {
		std::cerr << "Erreur lors de l'envoi du message." << std::endl;
		close(client_socket);
		return 1;
	}

	std::cout << "Message envoyé avec succès !" << std::endl;

	// Fermeture de la socket
	close(client_socket);

	return 0;
}
