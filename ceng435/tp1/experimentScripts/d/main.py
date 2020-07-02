import socket
import threading
import time
import subprocess as sp

# Messaging Port = 8080
# Controlling Port = 7070

def server(src_ip, src_port, dst_ip, dst_port):
    """
    Server thread implementation.
    Receives packet from r3 and send it back.
    Args:
        ip: IP of the machine that is connected
            to the corresponding client program.
        port: Port number that is connected to
              the corresponding client program.
    """
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_socket.bind((src_ip, src_port))
    end_to_end_delays = []
    sync_packet = "!".encode()# Packet indicating that an ntp sync is needed.
    while True:
        packet, _ = server_socket.recvfrom(1024)
        end = time.time()
        if packet == sync_packet:# It is a sync packet
            sp.call("sudo ntpdate -u pool.ntp.org", shell=True)# NTP
        server_socket.sendto(str(end).encode(), (dst_ip, dst_port))# Send time after receiving the packet to s for calculating end-to-end delay

def main():
    """
    Main function initializing the threads and killing them at the end.
    """
    t_server = threading.Thread(target=server, args=("10.10.7.1", 8080, "10.10.7.2", 8080))
    t_server.daemon = True# This ensures that the server thread will be killed after main is done
    t_server.start()
    termination_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    termination_socket.bind(("10.10.7.1", 7070))
    termination_socket.recvfrom(1024)# Wait for r3 to inform you that you and the server thread can die in peace

main()
