import threading
import socket
import time
import sys

# Messaging Port = 8080
# Controlling Port = 7070

# RTTs of the edges from r2 to r1, r3, s, and d.
rtts = {"r1": 0, "r3": 0, "s": 0, "d": 0}

def server_r1(ip, port):
    """
    Server thread implementation.
    Waits for the packet from the termination socket of r1.
    Args:
        ip: IP of the machine that is connected
            to the corresponding client program.
        port: Port number that is connected to
              the corresponding client program.
    """
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_socket.bind((ip, port))
    server_socket.recvfrom(1024)# Block until you get the "I am done measuring" signal from r1

def server_r3(ip, port):
    """
    Server thread implementation.
    Waits for the packet from the termination socket of r3.
    Args:
        ip: IP of the machine that is connected
            to the corresponding client program.
        port: Port number that is connected to
              the corresponding client program.
    """
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_socket.bind((ip, port))
    server_socket.recvfrom(1024)# Block until you get the "I am done measuring" signal from r3

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
    if (name == "r1" or name == "r3"):
        client_socket.sendto("!".encode(), (ip, port))
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
    t_server_r1 = threading.Thread(target=server_r1, args=("10.10.5.1", 7070))
    t_server_r3 = threading.Thread(target=server_r3, args=("10.10.6.1", 7070))
    t_client_r1 = threading.Thread(target=client, args=("r1", "10.10.4.1", 8080, N))
    t_client_r3 = threading.Thread(target=client, args=("r3", "10.10.6.2", 8080, N))
    t_client_s = threading.Thread(target=client, args=("s", "10.10.2.2", 8080, N))
    t_client_d = threading.Thread(target=client, args=("d", "10.10.5.2", 8080, N))
    t_server_r1.start()
    t_server_r3.start()
    t_client_r1.start()
    t_client_r3.start()
    t_client_s.start()
    t_client_d.start()
    t_client_s.join()
    t_client_d.join()
    t_client_r1.join()
    t_client_r3.join()
    f = open("link_costs.txt", "w+")# Write results to the file
    for i in rtts:
        out = "r2-" + i + " = " + str(rtts[i])
        print(out)
        f.write(out + "\n")
    f.close()
    t_server_r1.join()# Block until you get the termination signal from r1
    t_server_r3.join()# Block until you get the termination signal from r3
    # r1 and r3 are both done with measurin RTT and obviously r2 itself is also done.
    termination_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # Say everybody that "YOU SHALL DIE IN PEACE MY BROTHERS!"
    termination_socket.sendto("!".encode(), ("10.10.4.1", 7070))
    termination_socket.sendto("!".encode(), ("10.10.6.2", 7070))
    termination_socket.sendto("!".encode(), ("10.10.2.2", 7070))
    termination_socket.sendto("!".encode(), ("10.10.5.2", 7070))

main()
