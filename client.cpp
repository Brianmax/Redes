 /* Client code in C */
 
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
 #include <vector>
    
using namespace  std;
bool coma(string msg)
{
  int len = msg.size();
  for (int i = 0; i < len; i++)
  {
    if(msg[i] == ',')
      return true;
  }
  return false;
}

vector<string> getUserMsg(string msg)
{
  string nick, mensaje;
  int pos = msg.find(',');
  for (int i = pos + 1; i < msg.size(); i++)
  {
    mensaje = mensaje + msg[i];
  }
  for(int i = 0; i < pos; i++)
  {
    nick = nick + msg[i];
  }
  vector<string> g;
  g.push_back(mensaje);
  g.push_back(nick);
  return g;
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
      cout << buffer << ":";
    }
    else  if (accion == 'M'){
      n = read(socket_cliente,buffer,tamano);
      buffer[n]='\0';
      cout << buffer << "\n";
    }
    else if (accion == 'Q'){
      cout << "Se ha solicitado el cierre del chat!.\n";
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
  string name;
  int n;
  do{
    if (! nicknameOn){
      string a="N";
      cout << "Ingrese su Nickname:";
      cin >>  txt;
      name =  txt;
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
      //msg: pepe, hola como estas
      string a;
      if(coma(txt))
      {
        string user, msg;
        string tmp;
        vector<string> userMsg = getUserMsg(txt);
        a = "D";
        sprintf(buffer, "%03d", userMsg[0].size());
        buffer[4] = '\0';
        string t = buffer;
        tmp  = a + t + userMsg[0];
        char part2[200];
        sprintf(part2, "%02d", userMsg[1].size());
        part2[3] = '\0';
        string g = part2;
        tmp  = tmp + g + userMsg[1];
        strcpy(buffer, tmp.c_str());
        cout << "Protocolo: " << buffer << endl;
        n =  write(socket_cliente, buffer, tmp.size());
      }
      else if (txt == "list")
      {
        cout << "Entramos en list" <<endl;
        string a = "D";
        sprintf(buffer, "%03d", 4);
        //D004
        buffer[4] = '\0';
        string t = buffer;
        char buffer2[3]; sprintf(buffer2, "%02d", name.size());
        //05
        string nameSize = buffer2;
        string m =  a + t + "list" + nameSize + name;
        strcpy(buffer, m.c_str());
        cout << buffer << endl;
        n = write(socket_cliente, buffer, m.size());
      }
      else{
        a="M";
        sprintf(buffer,"%03d",txt.size());
        buffer[4]='\0';
        string t = buffer;
        string tmp = a +  t + txt;
        strcpy(buffer,tmp.c_str());
        cout << "Protocolo:" << tmp << endl;
        cout << "Protocolo buffer " << buffer << endl;
        n=write(socket_cliente,buffer,tmp.size());
      }
    }    
  }while(accion!='Q');
  shutdown(socket_cliente, SHUT_RDWR);
  close(socket_cliente);

}


  int main(void)
  {
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    int n;
    char buffer[1000];
 
    if (-1 == SocketFD)
    {
      perror("cannot create socket");
      exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    int puerto = 0;
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

  std::thread (read_thread,SocketFD).detach();
  std::thread (write_thread,SocketFD).detach();

  while(true){};

   // n = write(SocketFD,"Hi, this is Bob.",18);
    /* perform read write operations ... */
 

    return 0;
  }