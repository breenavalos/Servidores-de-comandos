/* 
 Cliente en Windows XP de mini Telnet de un servidor de comandos Debian 
 Hay que agregarle el ingreso de un usuario y contraseña 
 para que sea validado por el servidor. 
*/ 
#include <windows.h> 
#include <winsock2.h> //Cabecera para los socket windows 
#include <stdio.h> 
#include <stdlib.h> 
#define ipServidor 192.168.100.5 // ip del servidor
#define puertoServidor 8080  // puerto de escucha del servidor
void UbicacionDelServidor(struct in_addr); //descriptor de servidor

int main () 
{ 
    WSADATA wsa; ///Variable inicialización windows socket
    SOCKET dccliente; //Contiene el descriptor de socket cliente
    
	struct sockaddr_in remoto; //estructura de socket servidor, remoto
    
	WSAStartup (MAKEWORD(2,0),&wsa);//Inicializamos winsock para windows socket
	// Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Error al inicializar Winsock\n");
        return 1;
    }

	dccliente = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); // Obtiene el identificador / descriptor del socket
   
    if (dccliente == (unsigned)-1 ) 
     { 
          printf ("Error al crear el socket\n"); 
          exit(0); 
     } 
	 

    //Rellenamos la estructura sockaddr_in remoto 
    remoto.sin_family = AF_INET; //Dominio de Internet 
    remoto.sin_port = htons (puertoServidor); // puerto de escucha del servidor 
    remoto.sin_addr.s_addr = inet_addr(ipServidor); //direccion IPv4 donde esta corriendo el servidor 
    memset(remoto.sin_zero,0,8); //Completamos la estructura con ceros 
    //Establecemos conexion 
	
	
    if (connect(dccliente, (SOCKADDR *)&remoto, sizeof(SOCKADDR)) == -1) 
    { 
        printf("Error al conectar al servidor\n");
        return 1;
    } 
    //-------------------------------------- 
    printf ("Conexion establecida con exito...Datos del Servidor...\n"); 
    UbicacionDelServidor(remoto.sin_addr); 
    //---------------------------- 
    char bufin[512]; 
      printf("\nIngrese comando Linux "); 
      memset(bufin,0,512); 
      scanf("%s",bufin); 
	  // comparo cadena si es = 0; !=1 ;true = 1, false =0
      while(strcmp(bufin,"exit")) 
      {  // send solo funciona si se ha establecido conexion cliente-servidor
        //send envia / transmite mensaje "bufin" de sockect cliente a socket servidor
        //dccliente, descriptor, 
        //bufin,la cadena
        // strlen(bufin), la longitud en int
        //la bandera,especifica la forma en que se envia el mensaje, 0
        // si fallo devuelve -1, caso contrario numero de byte enviado 
        if (send(dccliente, bufin,strlen(bufin), 0) == -1) 
         { 
           printf("send fallo...\n"); 
           exit(0); 
         } 
         char buf[4096]; 
         int nb ; 
		  //recv, recibe mensaje del socket servidor
        //socket descriptor, 
        //buf, se alacena el mensaje
        //longitud en byte de buf
        //la bandera, especifica la forma en que recibe el mensaje,0
        // si fallo devuelve -1, caso contrario numero de byte recibido
         if ((nb=recv(dccliente, buf,4095, 0)) == -1) 
          { 
            printf("recv fallo...\n"); 
            exit(0); 
          } 
         buf[nb]='\0'; 
         printf("%s\n",buf); 
         printf("\nIngrese comando Linux "); 
         memset(bufin,0,512); 
         scanf("%s",bufin); 
    } 
    if (send(dccliente, bufin,strlen(bufin), 0) == -1) 
         { 
           printf("send fallo...\n"); 
           exit(0); 
         } 
    printf("Conexion terminada\n"); 
    system ("PAUSE"); 
    return (0); 
} 
 
//esta funcion es para mostrar el nombre de host si es que lo tiene
//gethostbyaddr obtiene la informacion y lo pasa al remoteHost tipo hostent
//caso contrario retorna un null, 0

void UbicacionDelServidor(struct in_addr addr ) 
{ 
  struct hostent *remoteHost; 
//  char *host_name; 
  char **pAlias; 
  remoteHost = gethostbyaddr((char *)&addr, 4, AF_INET); 
  if (remoteHost == NULL){
    printf("Fallo gethostbyaddr\n"); 
  else 
   { 
      printf("Nombre Servidor: %s\n", remoteHost->h_name); 
      for (pAlias = remoteHost->h_aliases; *pAlias != 0; pAlias++) 
       { 
            printf("Nombre Alternativo: %s\n", *pAlias); 
        } 
       printf("Tipo Address: "); 
       switch (remoteHost->h_addrtype) { 
       case AF_INET: 
            printf("AF_INET\n"); 
            break; 
       case AF_INET6: 
            printf("AF_INET6\n"); 
            break; 
       case AF_NETBIOS: 
            printf("AF_NETBIOS\n"); 
            break; 
       default: 
            printf(" %d\n", remoteHost->h_addrtype); 
            break; 
        } 
        printf("Bytes Address: %d\n", remoteHost->h_length); 
        if (remoteHost->h_addrtype == AF_INET) 
        { 
            int i = 0 ; 
            while (remoteHost->h_addr_list[i] != 0) 
             { 
                addr.s_addr = *(u_long *) remoteHost->h_addr_list[i++]; 
                printf("IPv4 Address #%d: %s\n", i, inet_ntoa(addr)); 
            } 
        } 
        else 
        if (remoteHost->h_addrtype == AF_INET6) printf("IPv6 address\n"); 
   } 
}



