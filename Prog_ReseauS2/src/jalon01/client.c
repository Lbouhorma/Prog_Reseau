#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/select.h>

#define SOCKET_ERROR -1
#define BIND_ERROR -1
#define INVALID_SOCKET -1
#define MESSAGE 50

//Coucou
ssize_t readline(char *buf) {
  scanf("%s", buf);
}

/*ssize_t sendline(int fd, const char *str) {
  int j = 0;
  while (str[j] != '\n') {
    write(fd, str[j] , 1);
    j++;
    fprintf(STDOUT_FILENO, "%d", str[j]);
  }
  return j;
} */

int main(int argc,char** argv)
{
  char msg_serveur[MESSAGE];
  char msg_client[MESSAGE];


    if (argc != 3)
    {
        fprintf(stderr,"usage: RE216_CLIENT hostname port\n");        //argv[]
        return 1;
    }

    struct sockaddr_in csin;
    struct sockaddr_in add_info_server;
    int sockc;


    sockc = socket(AF_INET,SOCK_STREAM, 0);       //Create the socket

    if (sockc == INVALID_SOCKET){                  //Check for validity
      printf("Erreur socket");
      exit(-1);
    }

    memset(&csin, '\0',sizeof(csin));
    csin.sin_family = AF_INET;                //Contexte d'adressage client
    csin.sin_port = htons(atoi(argv[2]));
    struct hostent* res;
    struct in_addr* addr;
    res = gethostbyname(argv[1]);
    addr = (struct in_addr*) res->h_addr_list[0];
    csin.sin_addr = *addr;

    fd_set readfs;
    /*if(int status = getaddrinfo(argv[1],argv[2],&sin,&add_info_server) != 0) {
      printf("%s\n", gai_sterror(status));
      exit(-3);
    }*/

    int connect_err = connect(sockc, (struct sockaddr *) & csin, sizeof(csin));       //Connect to the server
    if (connect_err == -1) {
      perror("ERROR address");
      exit(EXIT_FAILURE);
    }

    for(;;)
    {

      FD_ZERO(&readfs);
      FD_SET(sockc, &readfs);
      FD_SET(fileno(stdin), &readfs);
      int max_fd=sockc+1;


      int sel=select(max_fd, &readfs, NULL, NULL, NULL);

      int i;
      for(i=0; i<max_fd && sel>0; i++)
      {

        memset(msg_serveur,0,MESSAGE);
        memset(msg_client,0,MESSAGE);
        if(FD_ISSET(i, &readfs))
        {
          if(i==fileno(stdin))
          {
            read(i, msg_client, MESSAGE);
            write(sockc,msg_serveur,strlen(msg_serveur));
          }
          else
          {
            read(i, msg_client, MESSAGE);
            write(fileno(stdout),msg_serveur, strlen(msg_serveur));
          }
          sel--;
        }

      }
    }

    close(sockc);
    printf("fermeture du socket\n");

    return 0;


}
