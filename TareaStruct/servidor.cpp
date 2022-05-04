  /* Server code in C */
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

void functionRead(int connectFD, char buffer[256], int &n)
{
  for(;;){
    bzero(buffer,255);
    n = read(connectFD, buffer, 255);
    cout<<"                    "<<buffer<<endl;
  }
}

void functionWrite(int connectFD,char buffer[255], int &n)
{
  for(;;)
  {
    fgets (buffer, 255, stdin);
    n = write(connectFD, buffer, 255);
  }
}
int main(void)
{
  struct sockaddr_in stSockAddr;
  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  char buffer[255];
  int n;

  if(-1 == SocketFD)
  {
    perror("can not create socket");
    exit(EXIT_FAILURE);
  }

  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(45121);
  stSockAddr.sin_addr.s_addr = INADDR_ANY;

  if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
  {
    perror("error bind failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  if(-1 == listen(SocketFD, 10))
  {
    perror("error listen failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  for(;;)
  {
    int ConnectFD = accept(SocketFD, NULL, NULL);

    if(0 > ConnectFD)
    {
      perror("error accept failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    char messageToSend[255];
    thread (functionRead, ConnectFD, buffer, std::ref(n)).detach();
    thread (functionWrite, ConnectFD, messageToSend, std::ref(n)).detach();
    
      //Read
      //n = read(ConnectFD,buffer,255);
      functionRead(ConnectFD, buffer, n);
      //printf("             : [%s]\n",buffer);
      if (n < 0) perror("ERROR reading from socket");
      //Write
      //char messageToSend[255];
      //scanf("%s", messageToSend);
      //fgets (messageToSend, 255, stdin);
      //n = write(ConnectFD,messageToSend,18);
      functionWrite(ConnectFD, messageToSend, n);
      
      if (n < 0) perror("ERROR writing to socket");
    
   /* perform read write operations ... */

    shutdown(ConnectFD, SHUT_RDWR);

    close(ConnectFD);
  }

  close(SocketFD);
  return 0;
}