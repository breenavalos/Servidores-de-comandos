/* 
 Cliente en Windows XP de mini Telnet de un servidor de comandos Debian 
 Hay que agregarle el ingreso de un usuario y contraseña 
 para que sea validado por el servidor. 
*/ 
#include <stdio.h> 
#include <stdlib.h> 

#include <windows.h> 
#include <winsock2.h> //Cabecera para los socket windows 

#pragma comment(lib,"ws2_32.lib") 

#define SERVER_ADDR "192.168.100.5" 
#define PORT 6666 
void UbicacionDelServidor(struct sockaddr_in); 
int main () 
{ 
    WSADATA wsa; //Variable inicialización 
    SOCKET dccliente; //Contiene el descriptor de socket cliente 
    struct sockaddr_in remoto; //Contiene los datos de conexion del equipo remoto 
	
	// Inicializar Winsock
    WSAStartup (MAKEWORD(2,0),&wsa); //Inicializamos winsock 
	 if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Error al inicializar Winsock\n");
        return 1;
    }

	// Crear socket
    dccliente = socket (AF_INET, SOCK_STREAM, 0); // Obtiene el identificador del socket 
	if (dccliente == INVALID_SOCKET) {
        printf("Error al crear el socket\n");
        return 1;
    }

    //Rellenamos la estructura sockaddr_in remoto 
    remoto.sin_family = AF_INET; //Dominio de Internet 
    remoto.sin_port = htons(PORT); // puerto de escucha del servidor 
	remoto.sin_addr.s_addr = inet_addr(SERVER_ADDR); //direccion IPv4 donde esta corriendo el servidor
    memset(remoto.sin_zero,0,8); //Completamos la estructura con ceros 
    
	//Establecemos conexion 
    if (connect(dccliente, (struct sockaddr *)&remoto, sizeof(remoto)) == -1) 
    { 
        printf("Error al conectar al servidor\n");
        return 1;
    } 
    //-------------------------------------- 
    printf ("Conexion establecida con exito...Datos del Servidor...\n"); 
    UbicacionDelServidor(remoto);
    //---------------------------- 
    char bufin[512]; 
      printf("\nIngrese comando Linux "); 
      memset(bufin,0,512); 
      scanf("%s",bufin); 
      while(strcmp(bufin,"exit")) 
      { 
        if (send(dccliente, bufin,strlen(bufin), 0) == -1) 
         { 
           printf("send fallo...\n"); 
           exit(0); 
         } 
         char buf[4096]; 
         int nb ; 
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
 
 
void UbicacionDelServidor(struct sockaddr_in remoto) {
    printf("............c_sock.sin_family%d\n", remoto.sin_family);
    printf("............c_sock.sin_port%d\n", ntohs(remoto.sin_port));
    printf("............c_sock.sin_addr.s_addr%s\n", inet_ntoa(remoto.sin_addr));
}


