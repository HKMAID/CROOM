# Présentation
 Chatroom réalisé en langage C permettant à plusieurs clients de communiquer simultanément via un serveur central.
 
# Concepts marquants
 - Sockets réseau
 - Multithreading (pthread)

# Fonctionnement 
 - Vous commencez par lancer le serveur 
 - Entrez le nombre maximal de connexions (threads)
 - Ouvrez plusieurs terminaux, et lancez le programme client (éxecutez client.c) dans chaque un 
 - Insérez des coordonnées, ecrivez vos messages et vous allez voir les échanges sur le serveur dans un temps réel
 - L'exécution du serveur et des clients s’arrête manuellement
 - Le serveur se ferme une fois le nombre maximal de déconnexions atteint

# Prérequis
 - Ce projet est compatible uniquement avec les systèmes UNIX (Linux ou macOS)
 - Avoir un compilateur C (gcc recommandé)
