#cliente servidor comandos.c
#servidor ----
#puerto xxxx

import sys
import socket

def enviarComando(sockcli):
    comando =""
    comando=input('Enviar comando desde el Cliente al Servidor >> ')
    sockcli.send(comando.encode())
    return comando

def recibirMensaje(sockcli):
    salida=b''
    salida=sockcli.recv(8192)
    limpia=""
    for i, caracter in enumerate(salida):
        if caracter==0:
            break
        else:
            limpia +=chr(caracter)
    return limpia

########################################################
if __name__=='__main__':

    print("IP del servidor " + sys.argv[1])
    print("Puerto del servidor " + sys.argv[2])

    try:
        sockcli=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        sockcli.connect((sys.argv[1],int(sys.argv[2])))
    except:
        print("FALLO CONEXION CON EL SERVIDOR")
    else:
        while True:
            if enviarComando(sockcli)=="exit":break
            print(recibirMensaje(sockcli))

        print("termino la conexion")
        sockcli.close()
        