/*  
    Mini servidor secuencial de comandos Debian  
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
#include <signal.h>
#include <sys/wait.h>

#define IP "192.168.100.5" //Ubicacion del servidor mini telnet 
#define PUERTO 6666 

void mataZombies_Handler_De_SIGCHL(int);

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
  signal(SIGCHLD,mataZombies_Handler_De_SIGCHL);
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
			signal(SIGCHLD,SIG_DFL); //Si el hijo termina no quiere entrar a la funcion.
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
           char buf[30]; 
           int nb; 
           int defout ;                   
           nb = read(idsockc,buf,30); 
           while(strcmp(buf,"exit")) 
           { 
              buf[nb] = '\0'; 
              printf(".......recibido del cliente %d : %s\n",idsockc,buf); 
              defout = dup(1); 
              dup2(idsockc,1); 
              system(buf); 
              dup2(defout,1); 
              close(defout); 
              nb = read(idsockc,buf,30); 
          } 
}


void UbicacionDelCliente(struct sockaddr_in c_sock) 
{ 
  printf("............c_sock.sin_family %d\n",c_sock.sin_family); 
  printf("............c_sock.sin_port %d\n",c_sock.sin_port); 
  printf("............c_sock.sin_addr.s_addr %s\n\n", inet_ntoa(c_sock.sin_addr)); 
} 


void mataZombies_Handler_De_SIGCHL(int nrosig){
	printf("Entre a la funcion asociada a SIGCHLD %d\n",nrosig);
	//Ahora se debe capturar la señal. waitpid es bloqueante. Obtiene el estado de recuperacion del hijo.
	while (waitpid(-1,NULL,WNOHANG)>0); // Recupero procesos que terminaron que por tiempo pequeño son zombies. Se queda recurando Zombies.
}