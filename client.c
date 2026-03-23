#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct {
    char nom[32];
    int  age;
    char sexe;
    char message[180];
} user;

int main(void){
    int socketClient = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addrClient;
    addrClient.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrClient.sin_family = AF_INET;
    addrClient.sin_port = htons(3000);
    
    if (connect(socketClient, (const struct sockaddr *) &addrClient, sizeof(addrClient)) == -1) {
        perror("Erreur de connexion");
        close(socketClient);
        return -1;
    }

    user B;
    char msg[30];
    memset(&B, 0, sizeof(B));

    do {
        printf("Veuillez entrer votre nom : ");
        if (scanf("%31s", B.nom) == 1) break;
        fprintf(stderr, "Erreur : nom invalide.\n");
        int c; while ((c = getchar()) != '\n' && c != EOF);
    } while (1);

    do {
        printf("Veuillez entrer votre age : ");
        if (scanf("%d", &B.age) != 1) {
            fprintf(stderr, "Erreur : entrez un entier valide.\n");
            int c; while ((c = getchar()) != '\n' && c != EOF);
            B.age = 0;
            continue;
        }
        if (B.age < 15) {
            printf("Vous n'etes pas eligible (moins de 15 ans).\n");
            close(socketClient);
            return EXIT_FAILURE;
        }
    } while (B.age <= 0);

    do {
        printf("Veuillez entrer votre sexe (M/F) : ");
        if (scanf(" %c", &B.sexe) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
    } while (B.sexe != 'M' && B.sexe != 'm' && B.sexe != 'F' && B.sexe != 'f');

    // Paquet d'identification
    memset(B.message, 0, sizeof(B.message));
    if (send(socketClient, &B, sizeof(B), 0) < 0)
        perror("Erreur d'envoi du paquet d'identification");

    ssize_t premier = recv(socketClient, msg, sizeof(msg) - 1, 0);
    if (premier <= 0) {
        fprintf(stderr, "Connexion perdue avant le premier prompt.\n");
        close(socketClient);
        exit(1);
    }
    msg[premier] = '\0'; // Null-terminer le prompt reçu

    printf("Bienvenu %s !\n\n", B.nom);

    // Boucle de messages
    while (1) {
        printf("%s", msg);
        fflush(stdout);

        if (scanf(" %179[^\n]", B.message) != 1){
            fprintf(stderr, "Erreur de saisie : message trop long\n");
            break;
        }

        if (send(socketClient, &B, sizeof(B), 0) < 0) { // Envoyer le message au serveur
            perror("Erreur d'envoi du message");
            break;
        }
        
        // Attendre la réponse du serveur
        ssize_t prochain = recv(socketClient, msg, sizeof(msg) - 1, 0);
        if (prochain <= 0) {
            fprintf(stderr, "Connexion perdue.\n");
            break;
        }
        msg[prochain] = '\0';
    }

    printf("Déconnexion.\n");
    close(socketClient);
    return 0;
}
