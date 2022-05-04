 /* Client code in C */
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <iostream>
using namespace std;

void thread_read(int connectFD, char buffer[255], int &n)
{
  for(;;)
  {
    bzero(buffer, 255);
    n = read (connectFD, buffer, 255);
    cout<<"                    "<<buffer<<endl;
  }
}

void thread_write(int connectFD, char messageToSend[255], int &n)
{
  for(;;)
  {
    fgets (messageToSend, 255, stdin);
    n = write(connectFD, messageToSend, 255);
  }
}

int main(void)
{
  struct sockaddr_in stSockAddr;
  int Res;
  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  int n;

  if (-1 == SocketFD)
  {
    perror("cannot create socket");
    exit(EXIT_FAILURE);
  }

  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(45121);
  Res = inet_pton(AF_INET, "5.253.235.219", &stSockAddr.sin_addr);

  if (0 > Res)
  {
    perror("error: first parameter is not a valid address family");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }
  else if (0 == Res)
  {
    perror("char string (second parameter does not contain valid ipaddress");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }
    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
      perror("connect failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    char messageToSend[255];
    char toRead[255];
    thread (thread_write, SocketFD, messageToSend, std::ref(n)).detach();
    thread (thread_read, SocketFD, toRead, std::ref(n)).detach();
    while(1)
    {
      //Write
      thread_write(SocketFD, messageToSend, n);
      //scanf("%s", messageToSend);
      
      //fgets (messageToSend, 255, stdin);
      //n = write(SocketFD ,messageToSend ,18);
      
      //Read
      
      
      
      thread_read(SocketFD, toRead, n);
      //int n = read(SocketFD, toRead, 255);
      
      if (n < 0)
        cout<<"Error de lectura"<<endl;
      cout<<"Mensaje recibido: "<<toRead<<endl;
      memset(toRead, 0, 255);
    }
  /* perform read write operations ... */

  shutdown(SocketFD, SHUT_RDWR);

  close(SocketFD);
  return 0;
}