/*  
    Mini servidor concurrente de comandos con procesos pesados Debian  
*/

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 

//Bibliotecas de socket.
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 

#include <netdb.h> 

#define IP "192.168.100.5" //Ubicacion del servidor mini telnet 
#define PUERTO 8080 


void UbicacionDelCliente(struct sockaddr_in); 
void RecibeEnviaComandos(int); 

int main(int argc, char *argv[]) 
{ 
  struct sockaddr_in s_sock,c_sock; 
  int idsocks,idsockc; 
  int lensock = sizeof(struct sockaddr_in); 
  idsocks = socket(AF_INET, SOCK_STREAM, 0); 
  printf("idsocks %d\n",idsocks); 
  s_sock.sin_family      = AF_INET; 
  s_sock.sin_port        = htons(PUERTO); 
  s_sock.sin_addr.s_addr = inet_addr(IP); 
  memset(s_sock.sin_zero,0,8); 
  printf("bind %d\n", bind(idsocks,(struct sockaddr *) &s_sock,lensock)); 
  printf("listen %d\n",listen(idsocks,5)); 
  while(1) //Aca se define el tipo de serivdor - secuencial / concurrrente
    { 
      printf("esperando conexion\n");     
      idsockc = accept(idsocks,(struct sockaddr *)&c_sock,&lensock); 
      if(idsockc != -1) 
         { 
	  //Padre crea un hijo. Clonamos el proceso que ejecuta el fork en otro proceso
	  pid_t pid= fork();
	  
	  if ( pid==0)
	  { //Si el valor 0 esta en la variable PID entonces el hijo. En el padre va a estar asignado el ID del hijo.
	
           /* Ubicacion del Cliente */ 
            printf("conexion aceptada desde el cliente\n"); 
            UbicacionDelCliente(c_sock); 
           /*--------------------------------------------------*/

           RecibeEnviaComandos(idsockc); // Va a ser llamada por cada hijo
           
           close(idsockc); 
	   exit(0);
         } else
	  	if (pid == -1) printf("Fallo FORK () \n"); 
	}
      	else  
            printf("conexion rechazada %d \n",idsockc); 
        
    } exit(0); 
} 
 
void RecibeEnviaComandos(int idsockc) 
{ 
    char buf[512]; // Buffer para recibir comandos del cliente
    int nb; // N mero de bytes le dos
    int defout; // Descriptor de archivo para la salida est ndar
    nb = read(idsockc, buf, 512); // Leer el comando del cliente
    while (nb > 0 && strcmp(buf, "exit") != 0)
    {
        buf[nb] = '\0'; // A adir el car cter nulo al final del buffer para tratarlo como una cadena
        printf(".......recibido del cliente %d : %s\n", idsockc, buf);
        defout = dup(1);  // Duplicar el descriptor de archivo de la salida est ndar
        dup2(idsockc, 1); // Redirigir la salida est ndar al socket del cliente
        fflush(stdout); // Asegurarse de que el buffer se vac a antes de ejecutar el comando
        //system(buf); // Ejecutar el comando recibido
        int status = system(buf); // Ejecutar el comando recibido
        if (status == -1 || WEXITSTATUS(status) != 0) {
        // Si el comando falla, enviar un mensaje de error al cliente
        const char *error_msg = "Error. El comando ingresado no existe\n";
        write(idsockc, error_msg, strlen(error_msg));
        }
		dup2(defout, 1); // Restaurar la salida est ndar original
        close(defout); // Cerrar el descriptor de archivo duplicado
        memset(buf, 0, 512); // Limpiar el buffer antes de la pr xima lectura
        nb = read(idsockc, buf, 512); // Leer el siguiente comando del cliente
    }
}


void UbicacionDelCliente(struct sockaddr_in c_sock) 
{ 
  printf("............c_sock.sin_family %d\n",c_sock.sin_family); 
  printf("............c_sock.sin_port %d\n",c_sock.sin_port); 
  printf("............c_sock.sin_addr.s_addr %s\n\n", inet_ntoa(c_sock.sin_addr)); 
} 
