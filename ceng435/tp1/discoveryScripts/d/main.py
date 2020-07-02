import socket
import threading

# Messaging Port = 8080
# Controlling Port = 7070

def server(ip, port):
    """
    Server thread implementation.
    It directly sends back the received packet.
    Args:
        ip: IP of the machine that is connected
            to the corresponding client program.
        port: Port number that is connected to
              the corresponding client program.
    """
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 
    server_socket.bind((ip, port))
    while True:
        packet, addr = server_socket.recvfrom(1024)
        server_socket.sendto(packet, addr)

def main():
    t_server_r1 = threading.Thread(target=server, args=("10.10.4.2", 8080))
    t_server_r2 = threading.Thread(target=server, args=("10.10.5.2", 8080))
    t_server_r3 = threading.Thread(target=server, args=("10.10.7.1", 8080))
    t_server_r1.daemon = True# This ensures that the server thread will be killed after main is done
    t_server_r2.daemon = True# This ensures that the server thread will be killed after main is done
    t_server_r3.daemon = True# This ensures that the server thread will be killed after main is done
    t_server_r1.start()
    t_server_r2.start()
    t_server_r3.start()
    termination_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    termination_socket.bind(("10.10.5.2", 7070))# Let r2 know that you are done with measuring
    termination_socket.recvfrom(1024)# Wait for r2 to inform you that you and the server thread can die in peace

main()
