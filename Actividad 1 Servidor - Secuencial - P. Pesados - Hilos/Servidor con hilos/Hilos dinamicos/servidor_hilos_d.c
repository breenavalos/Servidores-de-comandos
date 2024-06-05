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

//Biblioteca de hilos
#include <pthread.h>

#define IP "192.168.100.5" //Ubicacion del servidor mini telnet 
#define PUERTO 6666 

void UbicacionDelCliente(struct sockaddr_in); 
//Codigo que va a ejecutar el hilo
void RecibeEnviaComandos(void *);

//No puede ser global por discrepancias entre los clientes aceptados por el servidor.
//int idsockc;

int main(int argc, char *argv[]) 
{ 
	if (argc !=3){
		printf("La sintaxis es --> comando IP PUERTO\n");
		exit(0);
	}
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
		//Creamos el hilo
		pthread_t hilo;
		//(DIRECCION DE HILO, LA DIRECCION DE MEMORIA DONDE VA A EJECUTAR EL HILO,argumentos que recibe la funcion)
		//casteo la direccion del cliente idsockc, para que reciba un puntero a void.
		pthread_create(&hilo,NULL,(void *)&RecibeEnviaComandos,(void *)&idsockc);
		  //Aca estoy en el servidor original
		  /* Ubicacion del Cliente */ 
            printf("conexion aceptada desde el cliente\n"); 
            UbicacionDelCliente(c_sock); 
           /*--------------------------------------------------*/     

	}
      	else  
            printf("conexion rechazada %d \n",idsockc); 
        
    } exit(0); 
} 
 
 //Estoy en el hilo
void RecibeEnviaComandos(void * id)  //Es un puntero a un tipo de dato void
{ 
		   int idsockc= *((int *)id);
		   printf("soy el hilo %lu atiendo al cliente %d\n",pthread_self(),idsockc);
           char buf[30]; 
		   char salida[8192];
		   int p[2];
           int nb; 
           int defout ;                   
           nb = read(idsockc,buf,30); 
           while(strcmp(buf,"exit")) 
           { // Agregamos pipe para evitar que se coloque en la pantalla y se escriba en el pipe.
			  pipe(p); //Creo un pipe. EL proceso que lo crea es escritor y lector
              buf[nb] = '\0'; 
              printf(".......recibido del cliente %d : %s\n",idsockc,buf); 
              defout = dup(1); 
              dup2(p[1],1); //El file descriptor de escritura en la posicion 1 y el de lectura en la posicion 0
              system(buf); 
              dup2(defout,1); 
              close(defout); 
			  // Para que no haya bloqueos lo que hay que hacer es cerrarlo. Cerrar el estremo de escritura. p[1]
			  close(p[1]); 
			  nb= read(p[0],salida,8192);
			  salida[nb]='\0';
			  close(p[0]); // se cierra el pipe creado
			  send(idsockc,salida,strlen(salida),0);
              nb = read(idsockc,buf,30); 
          } 
		  //El hilo cierra la conexion con ese cliente
		  close(idsockc); 
		  pthread_exit(NULL);
}

void UbicacionDelCliente(struct sockaddr_in c_sock) 
{ 
  printf("............c_sock.sin_family %d\n",c_sock.sin_family); 
  printf("............c_sock.sin_port %d\n",c_sock.sin_port); 
  printf("............c_sock.sin_addr.s_addr %s\n\n", inet_ntoa(c_sock.sin_addr)); 
} 

