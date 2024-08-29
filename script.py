import socket
import time

def induce_epoller_error(server_address, port):
    # Creación de un socket y conexión al servidor
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((server_address, port))

    # Envío datos
    sock.sendall(b"Hello, Server")

    # Sleep
    time.sleep(2)

    # Cerrar el socket abruptamente
    sock.close()

if __name__ == "__main__":
    induce_epoller_error('localhost', 7070) # Dirección y puerto
