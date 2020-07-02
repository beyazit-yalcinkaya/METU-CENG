import threading
import socket
import time

# Messaging Port = 8080
# Controlling Port = 7070

def server(src_ip, src_port, dst_ip, dst_port):
    """
    Server thread implementation.
    If receives packet from s and send it to d.
    If receives packet from d and send it to s.
    Args:
        ip: IP of the machine that is connected
            to the corresponding client program.
        port: Port number that is connected to
              the corresponding client program.
    """
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 
    server_socket.bind((src_ip, src_port))
    while True:
        data, addr = server_socket.recvfrom(1024)
        server_socket.sendto(data, (dst_ip, dst_port))

def main():
    """
    Main function initializing the threads and killing them at the end.
    """
    t_server_from_s_to_d = threading.Thread(target=server, args=("10.10.3.2", 8080, "10.10.7.1", 8080))
    t_server_from_d_to_s = threading.Thread(target=server, args=("10.10.7.2", 8080, "10.10.3.1", 8080))
    t_server_from_s_to_d.daemon = True# This ensures that the server thread will be killed after main is done
    t_server_from_d_to_s.daemon = True# This ensures that the server thread will be killed after main is done
    t_server_from_s_to_d.start()
    t_server_from_d_to_s.start()
    termination_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    termination_socket.bind(("10.10.3.2", 7070))
    termination_socket.recvfrom(1024)# Wait for s to inform you that you and the server thread can die in peace
    termination_socket.sendto("!".encode(), ("10.10.7.1", 7070))# Say d that "YOU SHALL DIE IN PEACE MY BROTHER!"

main()
