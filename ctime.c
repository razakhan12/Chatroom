/************* UDP CLIENT CODE *******************/
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdlib.h>
# define NUM_THREADS 2
typedef struct{
        int clientSocket;
} info; 
struct PDU {
                char	type;
                char  sname[10];
                char	data[100];
} rpdu, tpdu;

void *abcd(void * args)
{

  info *actual_args = args;
  int nBytes;
  int counter = 0;
  int parent_x, parent_y; int score_size = 3;
while(1)
{
  if(nBytes = recvfrom(actual_args->clientSocket,(char *)&rpdu,120,0,NULL, NULL) == -1)
    {
      printf("error recieving");
    }
 else if(rpdu.type == 'A')
    {
      printf("\n Acknowledged: %s:%s\n",rpdu.sname,rpdu.data);
    }
    else if(rpdu.type == 'E')
    {
    printf("\n %s:%s \n",rpdu.sname,rpdu.data);
    }
    else if(rpdu.type == 'B')
    {
      printf("\n %s:%s \n",rpdu.sname,rpdu.data);
    }
    else if(rpdu.type == 'U')
    {
      printf("\n Uni: %s \n",rpdu.data);
    }
      else if(rpdu.type == 'L')
    {
    printf("List: \n%s\n",rpdu.data);
    }
  }
}


int main(){
  
  pthread_t thread;
  int rc;
  int t;
  int clientSocket, portNum, nBytes;
  int counter = 0;
  char buffer[100];
    char unibuff[100];
  char halo[10];
  char uni[10];
  char cmd[10];
  int ch; 
  struct sockaddr_in serverAddr;
  socklen_t addr_size;	
  int parent_x, parent_y; int score_size = 3;    
 int menucount = 0;
  /*Create UDP socket*/
  clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
  /*Configure settings in address struct*/
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(7891);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
  /*Initialize size variable to be used later on*/
  info *args = malloc(sizeof *args);
  args->clientSocket = clientSocket;
  pthread_create(&thread, NULL, abcd, args);
  addr_size = sizeof serverAddr;
  printf("Type a username:\n");
  fgets(halo,10,stdin);
  printf("You typed: %s",halo);
  nBytes = strlen(halo) + 1;
  strcpy(tpdu.data, halo);
  strcpy(tpdu.sname, halo);
  tpdu.type = 'R';
  /*Send message to server*/
  sendto(clientSocket,(char *)&tpdu,120,0,(struct sockaddr *)&serverAddr,addr_size);
printf("Command: ");
while(1)
{
              scanf("%s",cmd);
              if(strcmp(cmd,"1") == 0) //send a message
              {
              printf("Sending a message: "); 
              scanf("%s",buffer);
              tpdu.type = 'M';
              strcpy(tpdu.data,buffer);
              strcpy(tpdu.sname,halo);
              sendto(clientSocket,(char *)&tpdu,120,0,(struct sockaddr *)&serverAddr,addr_size);
              memset(cmd,'0',sizeof(cmd));
              printf("Command:");
              }
              else if(strcmp(cmd,"2") == 0)//UNICAST
              {
              printf("Username: "); 
              scanf("%s",uni);
              printf("Sending a message: ");  
              scanf("%s",unibuff);
              tpdu.type = 'U';
              strcpy(tpdu.data,unibuff);
              strcpy(tpdu.sname,uni);
              sendto(clientSocket,(char *)&tpdu,120,0,(struct sockaddr *)&serverAddr,addr_size);
              memset(cmd,'0',sizeof(cmd));
              printf("Command:");
              }
               else if(strcmp(cmd,"3") == 0)//quit the list
              {
                tpdu.type = 'Q';
                strcpy(tpdu.data,"Quit");
                strcpy(tpdu.sname,halo);
                sendto(clientSocket,(char *)&tpdu,120,0,(struct sockaddr *)&serverAddr,addr_size);
                memset(cmd,'0',sizeof(cmd));
                printf("Command:");
              }
              else if(strcmp(cmd,"4") == 0)//request the list
              {
                tpdu.type = 'L';
                strcpy(tpdu.data,"Request List");
                strcpy(tpdu.sname,halo);
                sendto(clientSocket,(char *)&tpdu,120,0,(struct sockaddr *)&serverAddr,addr_size);
                memset(cmd,'0',sizeof(cmd));
              }
          }
  return 0;
}