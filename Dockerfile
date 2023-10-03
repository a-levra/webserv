# Utiliser une image de base Linux
FROM ubuntu:jammy

# Mettre à jour le système et installer les dépendances nécessaires
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    && rm -rf /var/lib/apt/lists/*


# Copier les fichiers sources de votre projet dans le conteneur
COPY . /app

# Définir le répertoire de travail
WORKDIR /app

# Compiler votre serveur web C++
RUN make

# Exposer le port 80 pour le serveur web
EXPOSE 8000

# Commande pour exécuter votre serveur web
CMD ["sh", "-c", "make re && ./webserv"]

# Remarque : Remplacez "./votre_serveur" par la commande réelle pour exécuter votre serveur web C++.
