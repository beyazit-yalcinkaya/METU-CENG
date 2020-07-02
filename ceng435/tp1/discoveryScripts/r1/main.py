import threading
import socket
import time
import sys

# Messaging Port = 8080
# Controlling Port = 7070

# This lock is needed to ensure that the client thread is
# created after the initial start signal of the r2 node.
lock = threading.Lock()
# RTTs of the edges from r1 to s and d.
rtts = {"s": 0, "d": 0}

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
    server_socket.recvfrom(1024)
    lock.release()# Ready to serve release the lock
    while True:
        packet, addr = server_socket.recvfrom(1024)
        server_socket.sendto(packet, addr)

def client(name, ip, port, N):
    """
    Client thread implementation.
    It sends N messages to the (ip, port).
    Args:
        name: Name of the node to which the packets will sent.
              This name is used for the rtts dictionary.
        ip: IP of the server machine.
        port: Port number used by the server machine.
        N: Number of samples for predicting RTT.
    """
    global rtts
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    dummy_packet = ".".encode()# A dummy packet to send
    for i in range(N):
        start = time.time()# Start measuring time
        client_socket.sendto(dummy_packet, (ip, port))
        client_socket.recvfrom(1024)
        end = time.time()# Stop measuring time
        rtts[name] += (end - start)# Accumulate each sampled RTT
    rtts[name] /= N# Average accumulated RTTs

def main():
    """
    Main function initializing the threads and killing them at the end.
    """
    global rtts
    N = int(sys.argv[1])
    lock.acquire()# Hold lock until the server is ready to serve
    t_server = threading.Thread(target=server, args=("10.10.4.1", 8080))
    t_server.daemon = True# This ensures that the server thread will be killed after main is done
    t_server.start()
    lock.acquire()# Wait for the server thread to get ready to serve
    lock.release()# The server thread is ready we are done with the lock
    t_client_s = threading.Thread(target=client, args=("s", "10.10.1.1", 8080, N))
    t_client_d = threading.Thread(target=client, args=("d", "10.10.4.2", 8080, N))
    t_client_s.start()
    t_client_d.start()
    t_client_s.join()
    t_client_d.join()
    f = open("link_costs.txt", "w+")# Write results to the file
    for i in rtts:
        out = "r1-" + i + " = " + str(rtts[i])
        f.write(out + "\n")
        print(out)
    f.close()
    termination_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    termination_socket.bind(("10.10.4.1", 7070))
    termination_socket.sendto("!".encode(), ("10.10.5.1", 7070))# Let r2 know that you are done with measuring
    termination_socket.recvfrom(1024)# Wait for r2 to inform you that you and the server thread can die in peace

main()
