| | | | |
|-|-|-|-|
|# Proyecto: Servidores de Comandos| | | |
|Este proyecto se basa en el desarrollo y configuración de un sistema cliente-servidor donde los clientes| implementados en Python y C++ en una máquina virtual con Windows| se comunican con servidores en una máquina virtual con Linux. Los servidores ejecutan comandos en Linux recibidos de los clientes y devuelven las respuestas. La comunicación entre clientes y servidores se realiza mediante sockets. Además| las máquinas virtuales están configuradas en una red interna con IPs específicas para facilitar la comunicación.|
|## Estructura del Proyecto| | | |
|### 1. Maquinas Virtuales| | | |
|- **Cliente (Windows)**| | | |
|  - Implementaciones en Python y C++.| | | |
|- **Servidor (Linux)**| | | |
|  - Servidores configurados para diferentes modelos de procesamiento:| | | |
|    - Secuencial| | | |
|    - Procesos pesados| | | |
|    - Hilos dinámicos| | | |
|    - Hilos estáticos| | | |
|### 2. Configuración de la Red| | | |
|Las máquinas virtuales están conectadas en una red interna configurada con IPs específicas para garantizar una comunicación eficiente.| | | |
|## Descripción de los Servidores| | | |
|### Servidor Secuencial| | | |
|Este servidor maneja una conexión a la vez| procesando cada solicitud de cliente de manera secuencial.| | |
|### Servidor de Procesos Pesados| | | |
|Este servidor crea un nuevo proceso para manejar cada solicitud del cliente| permitiendo así una mayor concurrencia a costa de un mayor uso de recursos.| | |
|### Servidor de Hilos Dinámicos| | | |
|Este servidor crea un nuevo hilo para cada solicitud del cliente| permitiendo una concurrencia más eficiente en comparación con los procesos pesados.| | |
|### Servidor de Hilos Estáticos| | | |
|Este servidor utiliza un pool de hilos pre-creados para manejar las solicitudes de los clientes| optimizando el uso de recursos y mejorando el rendimiento en comparación con la creación dinámica de hilos.| | |
|## Implementación del Cliente| | | |
|### Cliente en Python| | | |
|```python| | | |
|import socket| | | |
|def ejecutar_comando(comando):| | | |
|    with socket.socket(socket.AF_INET| socket.SOCK_STREAM) as s:| | |
|        s.connect(('IP_SERVIDOR'| PUERTO))| | |
|        s.sendall(comando.encode())| | | |
|        data = s.recv(1024)| | | |
|    print('Respuesta:'| data.decode())| | |
|if __name__ == "__main__":| | | |
|    comando = input("Ingrese el comando a ejecutar: ")| | | |
|    ejecutar_comando(comando)| | | |
|```| | | |
|### Cliente en C++| | | |
|```cpp| | | |
|#include <iostream>| | | |
|#include <string>| | | |
|#include <cstring>| | | |
|#include <sys/types.h>| | | |
|#include <sys/socket.h>| | | |
|#include <netinet/in.h>| | | |
|#include <unistd.h>| | | |
|#include <arpa/inet.h>| | | |
|void ejecutar_comando(const std::string& comando) {| | | |
|    int sock = socket(AF_INET| SOCK_STREAM| 0);| |
|    if (sock < 0) {| | | |
|        std::cerr << "Error al crear el socket" << std::endl;| | | |
|        return;| | | |
|    }| | | |
|    sockaddr_in serv_addr;| | | |
|    serv_addr.sin_family = AF_INET;| | | |
|    serv_addr.sin_port = htons(PUERTO);| | | |
|    inet_pton(AF_INET| "IP_SERVIDOR"| &serv_addr.sin_addr);| |
|    if (connect(sock| (struct sockaddr*)&serv_addr| sizeof(serv_addr)) < 0) {| |
|        std::cerr << "Error al conectar al servidor" << std::endl;| | | |
|        close(sock);| | | |
|        return;| | | |
|    }| | | |
|    send(sock| comando.c_str()| comando.length()| 0);|
|    char buffer[1024] = {0};| | | |
|    recv(sock| buffer| 1024| 0);|
|    std::cout << "Respuesta: " << buffer << std::endl;| | | |
|    close(sock);| | | |
|}| | | |
|int main() {| | | |
|    std::string comando;| | | |
|    std::cout << "Ingrese el comando a ejecutar: ";| | | |
|    std::getline(std::cin| comando);| | |
|    ejecutar_comando(comando);| | | |
|    return 0;| | | |
|}| | | |
|```| | | |
|## Implementación del Servidor| | | |
|### Ejemplo de Servidor en Python (Secuencial)| | | |
|```python| | | |
|import socket| | | |
|def manejar_cliente(conexion| direccion):| | |
|    print(f"Conectado con {direccion}")| | | |
|    comando = conexion.recv(1024).decode()| | | |
|    resultado = ejecutar_comando(comando)| | | |
|    conexion.sendall(resultado.encode())| | | |
|    conexion.close()| | | |
|def ejecutar_comando(comando):| | | |
|    import subprocess| | | |
|    resultado = subprocess.run(comando| shell=True| capture_output=True| text=True)|
|    return resultado.stdout| | | |
|def iniciar_servidor():| | | |
|    with socket.socket(socket.AF_INET| socket.SOCK_STREAM) as s:| | |
|        s.bind(('IP_SERVIDOR'| PUERTO))| | |
|        s.listen()| | | |
|        while True:| | | |
|            conexion| direccion = s.accept()| | |
|            manejar_cliente(conexion| direccion)| | |
|if __name__ == "__main__":| | | |
|    iniciar_servidor()| | | |
|```| | | |
|## Configuración de las Máquinas Virtuales| | | |
|1. **Configurar la Red Interna**:| | | |
|   - Asignar IP estáticas a ambas máquinas virtuales.| | | |
|   - Asegurarse de que ambas máquinas estén en la misma subred.| | | |
|2. **Instalación de Dependencias**:| | | |
|   - **Cliente Windows**: Instalar Python y herramientas de desarrollo C++.| | | |
|   - **Servidor Linux**: Instalar Python y configuraciones de red necesarias.| | | |
|3. **Prueba de Conectividad**:| | | |
|   - Verificar que las máquinas virtuales puedan comunicarse entre sí usando `ping`.| | | |
|## Uso del Sistema| | | |
|1. Iniciar el servidor en la máquina Linux.| | | |
|2. Ejecutar el cliente en la máquina Windows.| | | |
|3. Introducir el comando en el cliente y esperar la respuesta del servidor.| | | |
|## Contribuciones| | | |
|Para contribuir al proyecto| puedes clonar el repositorio| realizar tus cambios y enviar un pull request. Asegúrate de seguir las mejores prácticas de programación y documentar tus cambios adecuadamente.| |
|## Licencia| | | |
|Este proyecto está licenciado bajo la [MIT License](LICENSE).| | | |
|---| | | |
|Este README proporciona una guía clara y concisa sobre cómo configurar y utilizar el sistema cliente-servidor. Para más detalles| consulta los archivos de código fuente y la documentación adicional en el repositorio.| | |
