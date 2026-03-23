#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct user {
    char nom[32];
    int age;
    char sexe;
    char message[180];
} user;

void *fonc(void *arg) {
    int socket = *(int*)arg;
    const char msg[] = "Saisissez un message : "; // message à envoyer
    user A;
    memset(&A, 0, sizeof(A));

    // recevoir le paquet d'identification
    ssize_t n = recv(socket, &A, sizeof(A), 0);
    if (n <= 0) {
        fprintf(stderr, "Client déconnecté avant d'etre identifié.\n");
        close(socket);
        pthread_exit(NULL);
    }
    
    // verifier les chaines
    A.nom[sizeof(A.nom)-1]= '\0';
    A.message[sizeof(A.message)-1] ='\0';

    // message de connexion
    printf("%s a rejoint le chat.\n", A.nom);
    fflush(stdout); // forcer l'affichage
    
    while (1) {
        if (send(socket, msg, strlen(msg) + 1, 0) < 0) {
            perror("Erreur d'envoi du message");
            break;
        }

        // Recevoir le message du client
        n = recv(socket, &A, sizeof(A), 0);
        if (n <= 0) {
            printf("%s s'est déconnecté.\n", A.nom);
            fflush(stdout);
            break;
        }

        A.nom[sizeof(A.nom)-1]= '\0';
        A.message[sizeof(A.message) - 1] = '\0';

        printf("%s (%c|%d ans) : %s\n", A.nom, A.sexe, A.age, A.message); // Affichage du message reçu 
        fflush(stdout); 
    }

    close(socket);
    pthread_exit(NULL);
}

int main(void) {
    int socketServeur = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in addrServeur;
    addrServeur.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrServeur.sin_family = AF_INET;
    addrServeur.sin_port = htons(3000);
    
    // Vérification du bind 
    if (bind(socketServeur, (const struct sockaddr *) &addrServeur, sizeof(addrServeur)) < 0) {
        perror("Erreur Bind");
        close(socketServeur);
        return 1;
    }

    int n;
    do {
    printf("Veuillez entrer le nombre max de connexions au chat-room : ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        fprintf(stderr, "Erreur : entrez un chiffre valide.\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF); 
        n = 0;
        }
    } while (n <= 0);

    if (listen(socketServeur, n) < 0) {
        perror("Erreur d'écoute");
        close(socketServeur);
        return 0;
    }

    printf("Serveur lancé au port %d\n", ntohs(addrServeur.sin_port));
    
    pthread_t threads[n];
    int i;

    // Boucle d'acceptation et création des threads pour les n connexions
    for(i = 0; i < n; i++){
        struct sockaddr_in addrService;
        socklen_t Ssize = sizeof(addrService);
        
        int socketService = accept(socketServeur, (struct sockaddr*)&addrService, &Ssize);
        
        // Vérifier l'acceptation
        if (socketService < 0) {
            perror("Erreur d'acceptation");
            continue;
        }

        printf("Nouvelle connexion depuis %s\n",inet_ntoa(addrService.sin_addr));

        // Allocation de la mémoire pour passer le descripteur de socket au thread
        int *arg = malloc(sizeof(int));
        if (arg == NULL) {
            perror("Erreur allocation mémoire");
            close(socketService);
            continue;
        }
        *arg = socketService;
        
        if (pthread_create(&threads[i], NULL, fonc, arg) != 0) {
            perror("Erreur création thread");
            free(arg); // Libérer si la création échoue
        }
    }
    
    for(i = 0; i < n; i++){
        pthread_join(threads[i], NULL);
    }
    
    close(socketServeur);
    printf("Fin de discussion.\n");
    return 0; 
}