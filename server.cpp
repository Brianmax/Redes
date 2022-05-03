  /* Server code in C */
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>       // std::cout
#include <thread> 
#include <string>
#include <map>
#include <vector>
#include <fstream>

using namespace  std;

map<int , string> room;

void broadcast(string msg){
    int n; 
    char tamano[100];
    for (map<int,string>::iterator it=room.begin(); it!=room.end(); ++it){
      string a = "M";
      sprintf(tamano,"%03d",msg.size());
      tamano[4]='\0';
      string t = tamano;
      string buffer = a + t + msg ;
      n = write(it->first, buffer.c_str(), buffer.size());
      cout << "Protocolo:" << buffer << endl;

    }
}
int getInfo(string name)
{
  for (map<int,string>::iterator it=room.begin(); it!=room.end(); ++it){
      if (it->second == name)
        return it->first;
    }
  return 0;
}
vector<string> getUsers()
{
  vector<string> users;
  for (map<int,string>::iterator it=room.begin(); it!=room.end(); ++it)
      users.push_back(it->second);
  return users;
}
void ListCommand(vector<string> users, int socket, string nombre)
{
  int n;
  int port = getInfo(nombre);
  char message[1000];
  for (int i = 0; i < users.size(); i++)
  {
    sprintf(message, "%03d", users[i].size());
    string tmp = message;
    string m = "M" + tmp + users[i];
    strcpy(message, m.c_str());
    n = write(port, message, m.size());
  }
}
void read_thread(int socket_cliente) {
  char buffer[1000];
  char accion;
  int n, tamano;
  do{
    n = read(socket_cliente,buffer,4);
    accion = buffer[0];
    buffer[0]='0';
    buffer[4]='\0';
    tamano = atoi(buffer);
    if (accion == 'N'){
      n = read(socket_cliente,buffer,tamano);
      buffer[n]='\0';
      cout << "New Cliente:" << buffer << ":";
      room[socket_cliente] = buffer;
      broadcast(buffer);
    }
    else  if (accion == 'M'){
      n = read(socket_cliente,buffer,tamano);
      buffer[n]='\0';
      string message = buffer;
      cout << buffer << "\n";
      
      broadcast(buffer);
    }
    else if (accion == 'Q'){
      cout << "Se ha solicitado el cierre del chat!.\n";
      string nick = room[socket_cliente];
      room.erase (socket_cliente);
      n = write(socket_cliente,"Q000",4);
      broadcast("user se fue! " + nick); 
    }
    else if (accion == 'D')
    {
      char send[1000];
      char send2[1000];
      n = read(socket_cliente, buffer, tamano + 2);
      string tmp2 = buffer;
      string m = tmp2.substr(0,tamano);

      //Tam nombre
      string num = tmp2.substr(tamano, tmp2.size()-4);
      int newT = atoi(num.c_str());
      char buffer2[1000];
      //nombre
      n = read(socket_cliente, buffer2, newT);
      string nombre = buffer2;
      if (m == "list")
      {
        cout<<"Lista de usuarios: "<<endl;
        vector<string> users = getUsers();
        ListCommand(users, socket_cliente, nombre);
      }
      else{
        sprintf(send, "%03d", m.size());
        send2[0] = 'M';
        string copy1 = send2; //M
        string copy2 = send; //000
        copy1  = copy1 + copy2 + m;
        strcpy(send, copy1.c_str());
        int port = getInfo(nombre);
        cout << "Se esta enviando un mensaje a: "<< nombre << endl;
        n = write(port, send, copy1.size() + 1);
      }
      nombre.clear();
    }
    else if (accion == 'F')
    {
        cout << "Sending File" << endl;
        char text[tamano + 10];
        string tempo = text;
        text[tamano+9] = '\0';
        n = read(socket_cliente, text, 9 + tamano);
        tempo = text;
        /*for (map<int,string>::iterator it=room.begin(); it!=room.end(); ++it){
          n = write(it->first, text, tamano + 9 + 4);
          cout << "Protocolo: 45454545" << buffer << endl;
        }*/
        int tam = atoi(tempo.substr(tamano, 9).c_str());
        cout << "Tam file: " << tam << endl;
        cout << "Tamanio de llegada: " << n << endl;
        cout << "Protocolo de llegada: " << text << endl;
        fstream outfile("nuevo.txt", ios_base::app);
        for (int i = 0; i < tam/1024; i++)
        {
          char text2[1025];
          n = read(socket_cliente, text2, 1024);
          text2[1024] = '\0';
          cout << text2;
          outfile << text2;
          cout << "Size n: " << n << endl;
        }
        //Leer el resto del archivo
        char resto[tam%1024 + 1];
        n = read(socket_cliente, resto, tam%1024);
        cout << resto << endl;
        outfile << resto;
    }
  }while(accion != 'Q');

  shutdown(socket_cliente, SHUT_RDWR);
  close(socket_cliente);

  cout << "Read_thread termino.\n";
}
void write_thread(int socket_cliente) {
  char buffer[100];
  string  txt;
  char accion;
  bool  nicknameOn = false;
  int n;
  do{
    if (! nicknameOn){
      string a="N";
      cout << "Ingrese su Nickname:";
      cin >>  txt;
      nicknameOn = true;
      //buffer[0]='N';
      sprintf(buffer,"%03d",txt.size());
      buffer[4]='\0';
      string t = buffer;
      string tmp = a + t + txt;
      strcpy(buffer,tmp.c_str());
      cout << "Protocolo:" << tmp << endl;
      n = write(socket_cliente,buffer,tmp.size());
    }
    cout << "MSG: ";
    cin >> txt;
    if (txt.compare("Q") == 0 ){
      buffer[0]='Q';
      buffer[1]='0';
      buffer[2]='0';
      buffer[3]='0';
      n = write(socket_cliente,buffer,4);
      accion = 'Q';
    }
    else{ // msg
      string a="M";
      sprintf(buffer,"%03d",txt.size());
      buffer[4]='\0';
      string t = buffer;
      string tmp = a +  t + txt;
      strcpy(buffer,tmp.c_str());
      cout << "Protocolo:" << tmp << endl;
      n=write(socket_cliente,buffer,tmp.size());
    }    
  }while(accion!='Q');
  shutdown(socket_cliente, SHUT_RDWR);
  close(socket_cliente);

}




  int main(void)
  {
    struct sockaddr_in stSockAddr;
    int SocketServer = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    char buffer[256];
    int n;
 
    if(-1 == SocketServer)
    {
      perror("can not create socket");
      exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1100);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;
 
    if(-1 == bind(SocketServer,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
      perror("error bind failed");
      close(SocketServer);
      exit(EXIT_FAILURE);
    }
 
    if(-1 == listen(SocketServer, 10))
    {
      perror("error listen failed");
      close(SocketServer);
      exit(EXIT_FAILURE);
    }
 
    for(;;) // muchos  clientes      
    {
      int newSocketCli = accept(SocketServer, NULL, NULL);
      if(0 > newSocketCli)
      {
        perror("error accept failed");
        close(SocketServer);
        exit(EXIT_FAILURE);
      }
 
   std::thread (read_thread,newSocketCli).detach();
   std::thread (write_thread,newSocketCli).detach();

    }
 
    close(SocketServer);
    return 0;
  }
