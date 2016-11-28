#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


int numb = 0;
typedef struct node{
        char username[10];
        struct sockaddr_storage sockaddr;
        struct node * next;
     }chatList;
chatList *head = NULL;
chatList *current = NULL;

void push(char * username,  struct sockaddr_storage json) //Add item to the linked list
{
   chatList *link =  malloc(sizeof(chatList));
   strcpy(link->username,username);
   link->sockaddr = json;
   //point it to old first node
   link->next = head;
   //point first to new first node
   head = link;
}

void printList() { // print items from the linked list
   chatList *amp = head;
   printf("\nUpdated List: [ ");
   //start from the beginning
   while(amp != NULL) {
      printf("(%s,) ",amp->username);
      amp = amp->next;
   }
   printf(" ]");
}

chatList* delete(char * a) //delete a username form the linked list
{
   chatList* now = head;
   chatList* previous = NULL;
   chatList* temp = malloc(sizeof(chatList));
   if(head == NULL) {
      return NULL;
   }
   while(strcmp(now->username,a) != 0) {// Compares the list
      if(now->next == NULL) {
         return NULL;
      } else {
          previous = now;
         now = now->next;
      }
   }if(now == head) {
      head = head->next;
   } else {
      previous->next = now->next;
   }    
   return now;
}
int checkName(char * a) {
   chatList *ptr = head;
   chatList *temp = malloc(sizeof(chatList));
   //start from the beginning
   while(ptr != NULL) {
      if(strcmp(ptr->username,a) == 0)
      {
          return 1;
      }
      ptr = ptr->next;
   }
   return 0;
}

int main()
{
    struct PDU {
                char	type;
                char    sname[10];
                char	data[100];
        } rpdu, tpdu;
        
  int udpSocket, nBytes;
  char buffer[100];
  char buf[100];
  struct sockaddr_in serverAddr, clientAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size, client_addr_size;
  int i;
  /*Create UDP socket*/
  udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
  /*Configure settings in address struct*/
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(7891);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
  /*Bind socket with address struct*/
  bind(udpSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
  /*Initialize size variable to be used later on*/
  addr_size = sizeof clientAddr;
  while(1)
  {
    /* Try to receive any incoming UDP datagram. Address and port of
      requesting client will be stored on serverStorage variable */
if((nBytes = recvfrom(udpSocket,(char *)&rpdu,120,0,(struct sockaddr *)&serverStorage, &addr_size)) == -1)
            {
              printf("error recieving");
             // exit(1);
            }
            char a = rpdu.type;
            char *name = rpdu.sname;
            switch(a){
                 case 'L': // Sending a List to the client
                     printf("Sending List:");
                 chatList *al = head;
                          while(al != NULL)
                          {
                          printf("Got message from %s", rpdu.sname);
                          tpdu.type = 'L';                 //Broadcast
                          strcpy(tpdu.sname,rpdu.sname); //username
                          strcat(tpdu.data,al->username);
                          al = al->next;
                          }
                   sendto(udpSocket,(char *)&tpdu,120,0,(struct sockaddr *)&serverStorage,addr_size);
                   printf("Sent List to %s", rpdu.sname);
                  printf("List requested"); 
                 break;
                 case 'M': //Recieved Message
                printf("Messaging:");
                 chatList *ptr = head;
                          while(ptr != NULL)
                          {
                          printf("Got message from %s", rpdu.sname);
                          tpdu.type = 'B';                  //Broadcast
                          strcpy(tpdu.sname,rpdu.sname);    //username
                          strcpy(tpdu.data,rpdu.data);
                          sendto(udpSocket,(char *)&tpdu,120,0,(struct sockaddr *)&ptr->sockaddr,addr_size);
                          ptr = ptr->next;
                          }
                   printf("Sent Message to %s", rpdu.sname);
                 break;
                case 'U':
                printf("Sending Unicast:");//Sending the Unicast
  	                      chatList *ball = head;
                          while(ball != NULL) 
                          {
                          int a = strcmp(ball->username,name);
                          if(a == 10)
                          {
                          tpdu.type = 'U';                 //Broadcast
                          strcpy(tpdu.sname,rpdu.sname); //username
                          strcpy(tpdu.data,rpdu.data);
                          sendto(udpSocket,(char *)&tpdu,120,0,(struct sockaddr *)&ball->sockaddr,addr_size);
                          }
                          ball = ball->next;
                        }
                break;
                case 'R'://Recieved request to be registered
                 printf("Received registration request from: %s\n",rpdu.sname);
                   if((checkName(rpdu.sname)) == 1)
                   {
                   	  printf("Checking if %s already exists",rpdu.sname);
                      strcpy(tpdu.sname,rpdu.sname); //username
                      tpdu.type = 'E';                 //error
                      strcpy(tpdu.data,"Unable to register");  //Unable to register
                      sendto(udpSocket,(char *)&tpdu,120,0,(struct sockaddr *)&serverStorage,addr_size);
                   }
                    else
                   {//Trying to register
                   	  printf("Trying to register %s",rpdu.sname);
                      push(rpdu.sname,serverStorage); // add the username and the ipaddress to the linkedlist
                      strcpy(tpdu.sname,rpdu.sname); //username
                      tpdu.type = 'A';                 //error
                      strcpy(tpdu.data,"Registered");  //Unable to register
                      sendto(udpSocket,(char *)&tpdu,120,0,(struct sockaddr *)&serverStorage,addr_size);
        			  //sending a broadcast message to everyone
        			   chatList *sam = head;
                                  while(sam != NULL) {
                                  printf("Got message from %s", rpdu.sname);
                                  tpdu.type = 'B';                 //Broadcast
                                  strcpy(tpdu.sname,rpdu.sname); //username
                                  strcpy(tpdu.data,rpdu.data);
                                  sendto(udpSocket,(char *)&tpdu,120,0,(struct sockaddr *)&sam->sockaddr,addr_size);
                                  sam = sam->next;
                               }
                   }
                 break;
                 case 'Q':// Request to quit the chat
                 delete(name);
                 printList();
                 chatList *any = head;
                 while(any != NULL) {
                       printf("Got message from %s", rpdu.sname);
                       tpdu.type = 'B';                 //Broadcast
                       strcpy(tpdu.sname,rpdu.sname); //username
                       strcpy(tpdu.data,strcat(rpdu.sname," has left chat"));
                       sendto(udpSocket,(char *)&tpdu,120,0,(struct sockaddr *)&any->sockaddr,addr_size);
                       any = any->next;
                 }
                 printf("Removed");
                 break;
                default:
                printf("polling inside switch"); //Still polling the switch
       }
	printf("\n polling out side of switch \n");
}
  return 0;
}
