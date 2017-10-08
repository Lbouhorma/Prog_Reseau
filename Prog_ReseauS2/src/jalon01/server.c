#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<netdb.h>

#define SOCKET_ERROR -1
#define BIND_ERROR -1
#define INVALID_SOCKET -1
#define MESSAGE 50

struct sockaddr_in csin;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char* argv[])
{

    if (argc != 2)
    {
        fprintf(stderr, "usage: RE216_SERVER port\n");      //argv[]
        return 1;
    }



    //init the serv_add structure
      struct sockaddr_in sin;
      struct sockaddr_in csin;
      int sockfd;

      sockfd = socket( AF_INET, SOCK_STREAM, 0 );             //Create the socket
      if (sockfd == INVALID_SOCKET){                          //Check for validity
        perror( "socket" );
        exit(EXIT_FAILURE);
      }

      memset(&sin, 0,sizeof(sin));
      sin.sin_addr.s_addr = htonl(INADDR_ANY);              //Contexte d'adressage serveur
      sin.sin_family = AF_INET;
      sin.sin_port = htons(atoi(argv[1]));

        int bind_err = bind(sockfd , (struct sockaddr *)&sin, sizeof(sin));     //Perform the binding and we bind on the tcp port specified
        if (bind_err == BIND_ERROR) {                                           //Verification
          perror ("bind");
          exit (EXIT_FAILURE);
        }

        //Specify the socket to be a server socket and listen for at most 20 concurrent client

        int listen_err = listen(sockfd, 20);        //On met la socket serveur sur écoute en attente d'une connexion
        if (listen_err == SOCKET_ERROR) {           //Verification
          perror("listen");
          exit (EXIT_FAILURE);
        }

        fd_set readfs;
        int clients[20];
        int actual = 0;

        for (;;)
        {
          printf("Connexion au serveur\n");

          int res = 0;
          int nv_client = 0;
          FD_ZERO(&readfs);                       // Créé un ensemble pour le préparer à recevoir des descripteurs de fichiers
          FD_SET(sockfd, &readfs);                //On rajoute le descripeur de fichier pour la connection coté serveur
          FD_SET(STDOUT_FILENO, &readfs);         //On rajoute le descripteur de fichier pour l'entrée utilisateur
          int max_fd = sockfd + 1;                // max_fd prendre le descripteur de fichier avec le plus grand numéro + 1
          int sel;
          /*
          int i;
          for (i=0; i <nv_client+1 && sel>0; i++){        //On rajoute le nouveau client dans le tablea des clients
            FD_SET(clients[i], &readfs);
          }*/

          sel=select(max_fd, &readfs, NULL, NULL, NULL);    //select est la pour survveiller l'ensemble de descripteurs readfs au cas ou de nouveaux caractères sont disponibles à la lecture
          if(sel<0){
            perror("select()");
            exit (EXIT_FAILURE);
          }

          char buf[MESSAGE];
          memset(buf,0,MESSAGE);

          if(FD_ISSET(sockfd, &readfs))
          {
              struct sockaddr_in csin = {0};
              socklen_t taille = sizeof(csin);
              int sockc = accept(sockfd, (struct sockaddr*)&csin, &taille);     //On accepte un nouveau client
              if(sockc == SOCKET_ERROR)
              {
                perror("accept()");
                continue;
              }

              printf("Connexion réussie\n");

              FD_SET(clients[actual], &readfs);               //On ajoute le descripteur de fichier du nouveau client dans readfs
              clients[actual]=sockc;
              int r=read(clients[actual], buf,MESSAGE);     //On lit ce que le client envoie
              printf("%d\n",r);
              if(r == -1){
                continue;
              }
              else if(r>0){
                write(clients[actual],buf,MESSAGE);         //On écrit ce que le client envoie
                printf("%s\n",buf);
              }

              if (sockc>max_fd){
                max_fd=sockc;
              }
              FD_SET(sockc, &readfs);       //On rajoute sockc dans l'ensemble readfs

              actual++;
          }
          else
          {
              int i = 0;
              for(i = 0; i < actual; i++)
              {
                if(FD_ISSET(clients[i],&readfs))        //On regarde si le descripteur de fichier du client est bien dans readfs
                {

                  int c = read(clients[i], buf,MESSAGE);     //On renvoie le message reçu au client
                  write(clients[i],buf,MESSAGE);
                  printf("%s\n",buf);
                  close(clients[i]);
                }
              }
          }

      }


    //Clean up server socket
    printf("Fermeture de la socket...\n");
    close(sockfd);
    printf("Fermeture du serveur terminee\n");

    return 0;

}
