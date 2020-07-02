import threading
import socket

def server(src_ip, src_port, dst_ip, dst_port):
    """
    Server thread implementation.
    If receives packet from src and send it to dst.
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
    t_server_from_s_to_d.join()
    t_server_from_d_to_s.join()

main()
