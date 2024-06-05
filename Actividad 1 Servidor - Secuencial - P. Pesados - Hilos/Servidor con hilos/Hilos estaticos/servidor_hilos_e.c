#include <stdio.h>       
#include <stdlib.h>
#include <pthread.h>      
#include <unistd.h>   
#include <string.h>   
#include <fcntl.h>   
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>       
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_THREADS 2 // Define el número máximo de hilos estáticos

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Definición de un nodo de la lista enlazada
typedef struct Node {
    int data;
    struct Node* next;
} Node;

// Definición de la cola
typedef struct {
    Node *front, *rear;
    int size; // Tamaño de la cola
} Queue;

Queue sockQueue = { .front = NULL, .rear = NULL, .size = 0 }; // Inicialización de la cola. `sockQueue` es una instancia de la cola, inicialmente vacía. 


/* Se utiliza una cola para gestionar las conexiones entrantes, y esta cola está implementada utilizando una lista enlazada simple. La cola se gestiona mediante dos punteros.
front: Apunta al primer nodo de la cola (el nodo más antiguo).
rear: Apunta al último nodo de la cola (el nodo más reciente).
*/

void enqueue(int sock) {//`enqueue` añade un nuevo socket a la cola. La función enqueue añade un nuevo nodo (que contiene el descriptor del socket) al final de la cola:
    Node *newNode = (Node*)malloc(sizeof(Node)); //Se crea un nuevo nodo con el descriptor del socket (newNode->data = sock).
    newNode->data = sock;
    newNode->next = NULL;
    if (sockQueue.rear == NULL) {//Si la cola está vacía (sockQueue.rear == NULL), tanto el front como el rear de la cola apuntan al nuevo nodo.
        sockQueue.front = newNode;
        sockQueue.rear = newNode;
    } else {//Si la cola no está vacía, el nuevo nodo se añade al final y se actualiza el puntero rear.
        sockQueue.rear->next = newNode;
        sockQueue.rear = newNode;
    }
    sockQueue.size++;
	printf("Cliente encolado en espera.\n");
	
}

int dequeue() {//`dequeue` elimina un socket de la cola y lo devuelve. 
    if (sockQueue.front == NULL) {
        return -1; // Cola vacía.Si la cola está vacía, devuelve -1.
    }
	////Si la cola no está vacía, obtiene el descriptor del socket del nodo en el frente, ajusta el puntero front y libera el nodo.
    int sock = sockQueue.front->data;
    Node *temp = sockQueue.front;
    sockQueue.front = sockQueue.front->next;
    if (sockQueue.front == NULL) { // Si la cola queda vacía después de desencolar, también ajusta el puntero rear a NULL.
        sockQueue.rear = NULL; 
    }
    free(temp);
    sockQueue.size--;
    return sock;
}

void UbicacionDelCliente(struct sockaddr_in); 
void *RecibeEnviaComandos(void *); 

int main(int argc, char *argv[]) {
    if (argc != 3 ) {
       printf("sintaxis error ingrese ./comando ip puerto\n");
       exit(-1);
    }

    socklen_t addrlen ;
    int sockfd ;
    struct sockaddr_in addr_in , addrcli_in ; 
    sockfd = socket(AF_INET, SOCK_STREAM , 0);
    printf("socket = %d\n",sockfd);
     
    addr_in.sin_family = AF_INET ;
    addr_in.sin_port   = htons(atoi(argv[2])) ;
    addr_in.sin_addr.s_addr  = inet_addr(argv[1]) ;
    memset(addr_in.sin_zero,0,8) ;
      
    addrlen = sizeof(addr_in);
    int bn = bind(sockfd, (struct sockaddr *)&addr_in , addrlen);
    printf("bind = %d\n",bn);

    int lst = listen(sockfd,5) ;
    printf("listen = %d\n",lst);

    pthread_t thread_pool[MAX_THREADS]; //Estos hilos continuamente intentan obtener un socket de la cola para procesar las solicitudes del cliente.

    for (int i = 0; i < MAX_THREADS; ++i) {
        pthread_create(&thread_pool[i], NULL, RecibeEnviaComandos, NULL);
    }

    while(1) {
        // Verificar si hay menos clientes conectados que el número máximo de hilos
        if (sockQueue.size < MAX_THREADS) {
            printf("servidor en puerto %s esperando conexion del cliente\n",argv[2]);
			//Cuando el servidor acepta una nueva conexión entrante, el socket de la conexión se añade a la cola. 
            int sockclifd = accept(sockfd, (struct sockaddr *)&addrcli_in, &addrlen);
            if (sockclifd > 0) { //Si la conexión se acepta correctamente (sockclifd > 0)
                pthread_mutex_lock(&mutex); //se bloquea el mutex (pthread_mutex_lock(&mutex)) para asegurar que el acceso a la cola es seguro.
                enqueue(sockclifd);  // Se llama a la función enqueue(sockclifd) para agregar el nuevo socket a la cola.
                pthread_mutex_unlock(&mutex);  //se desbloquea el mutex (pthread_mutex_unlock(&mutex)).
                UbicacionDelCliente(addrcli_in);
            } else { 
                printf("conexion rechazada %d \n",sockclifd); 
            } 
        } else {
            // Si hay suficientes clientes conectados, esperar un tiempo antes de volver a verificar
            usleep(10000); // Esperar un corto tiempo para evitar bloqueo de CPU
        }
    }     
    exit(0);
} 

void *RecibeEnviaComandos(void *arg) {
    while(1) {
        pthread_mutex_lock(&mutex);
        int idsockcli = dequeue(); // Obtener socket de la cola. Obtener socket de la cola.  intenta desencolar un socket de la cola usando dequeue().Si dequeue devuelve un socket válido (idsockcli != -1), el hilo procede a procesar la conexión del cliente.
        pthread_mutex_unlock(&mutex);

        if (idsockcli != -1) {
            printf("haciendo lo del comando\n");
            char comando[30];
            char salida[8192];
            int p[2];
            int nb; 
            int defout ; 
            while(1) {
                nb = recv(idsockcli, comando, 30, 0); 
                if (nb <= 0) {
                    break;
                }
                comando[nb] = '\0'; 
                printf(".......recibido del cliente %d : %s\n", idsockcli, comando); 
                if (strncmp(comando, "exit", 4) == 0) {
                    break;
                }
                pipe(p);    
                defout = dup(1); 
                dup2(p[1], 1); 
                system(comando); 
                dup2(defout, 1); 
                close(defout);
                close(p[1]);
                memset(salida, '\0', 8192);
                nb = read(p[0], salida, 8192);
                salida[nb] = '\0';
                close(p[0]);
                send(idsockcli, salida, strlen(salida), 0);
            }
            close(idsockcli);
       // } else {
            //usleep(10000); // Esperar un corto tiempo para evitar bloqueo de CPU
        }
    }
    pthread_exit(NULL);
} 

void UbicacionDelCliente(struct sockaddr_in c_sock) {
    printf("............c_sock.sin_family %d\n",c_sock.sin_family); 
    printf("............c_sock.sin_port %d\n",c_sock.sin_port); 
    printf("............c_sock.sin_addr.s_addr %s\n\n", inet_ntoa(c_sock.sin_addr)); 
}
