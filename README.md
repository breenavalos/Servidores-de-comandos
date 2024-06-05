# Proyecto: Servidores de Comandos

Este proyecto se basa en el desarrollo y configuración de un sistema cliente-servidor donde los clientes, implementados en Python y C++ en una máquina virtual con Windows, se comunican con servidores en una máquina virtual con Linux. Los servidores ejecutan comandos en Linux recibidos de los clientes y devuelven las respuestas. La comunicación entre clientes y servidores se realiza mediante sockets. Además, las máquinas virtuales están configuradas en una red interna con IPs específicas para facilitar la comunicación.

## Estructura del Proyecto

### 1. Maquinas Virtuales

- **Cliente (Windows)**
  - Implementaciones en Python y C++.
- **Servidor (Linux)**
  - Servidores configurados para diferentes modelos de procesamiento:
    - Secuencial
    - Procesos pesados
    - Hilos dinámicos
    - Hilos estáticos

### 2. Configuración de la Red

Las máquinas virtuales están conectadas en una red interna configurada con IPs específicas para garantizar la comunicación.

## Descripción de los Servidores

### Servidor Secuencial

Este servidor maneja una conexión a la vez, procesando cada solicitud de cliente de manera secuencial.

### Servidor de Procesos Pesados

Este servidor crea un nuevo proceso para manejar cada solicitud del cliente, permitiendo así una mayor concurrencia a costa de un mayor uso de recursos.

### Servidor de Hilos Dinámicos

Este servidor crea un nuevo hilo para cada solicitud del cliente, permitiendo una concurrencia más eficiente en comparación con los procesos pesados.

### Servidor de Hilos Estáticos

Este servidor utiliza un pool de hilos pre-creados para manejar las solicitudes de los clientes, optimizando el uso de recursos y mejorando el rendimiento en comparación con la creación dinámica de hilos.


## Uso del Sistema

1. Iniciar el servidor en la máquina Linux.
2. Iniciar el cliente en la máquina Windows.
3. Ejecutar el servidor 
4. Ejecutar el o los clientes.
