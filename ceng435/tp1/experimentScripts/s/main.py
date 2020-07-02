import socket
import threading
import time
import subprocess as sp
import numpy as np
import sys

# Messaging Port = 8080
# Controlling Port = 7070

# N * M gives the number of samples
# There are at least 1000 samples
# Before each 1000 samples NTP is performed to sync
N = 1
M = 1000
end_to_end_delays = []# List of measured end to end delays

def client(src_ip, src_port, dst_ip, dst_port):
    """
    Client thread implementation.
    Receives packets from (src_ip, src_port).
    It sends N * M messages to the (dst_ip, dst_port).
    Args:
        name: Name of the node to which the packets will sent.
              This name is used for the end-to-end dictionary.
        ip: IP of the server machine.
        port: Port number used by the server machine.
        N: Number of samples for predicting end-to-end delay.
    """
    global N, M, end_to_end_delays
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    client_socket.bind((src_ip, src_port))
    sync_packet = "!".encode()# Sync message for NTP its end-to-end delay is not measured
    dummy_packet = ".".encode()# Dummy packet to measure end-to-end delay
    for i in range(N):# NTP N times
        # The following order of NTP and informing d to perform NTP is safe
        # It ensures that no deadlock occurs
        sp.call("sudo ntpdate -u pool.ntp.org", shell=True)# Update NTP
        client_socket.sendto(sync_packet, (dst_ip, dst_port))# Say d to update NTP
        client_socket.recvfrom(1024)# Wait until d is done with NTP
        for j in range(M):# Send M dummy packets and measure end-to-end delay
            start = time.time()# Start measuring time
            client_socket.sendto(dummy_packet, (dst_ip, dst_port))
            end, _ = client_socket.recvfrom(1024)# Get stop time from d
            end = float(end.decode())# Decode and convert float
            end_to_end_delays.append(end - start)# Save measured end-to-end delay

def main():
    """
    Main function initializing the threads and killing them at the end.
    """
    global N, M
    # M is always 1000 because performing NTP for each 1000 packets does not
    # exceed the NTP rate limit and it ensures sync between s and d
    N = int(sys.argv[1]) / M# Find N
    N = 1 if N == 0 else int(np.ceil(N))# N it at least 1 and it is ceiled
    exp_name = sys.argv[2]# exp1, exp2, exp3
    t_client = threading.Thread(target=client, args=("10.10.3.1", 8080, "10.10.3.2", 8080))
    t_client.start()
    t_client.join()
    out = "N = " + str(N * M) + "\n" + "Mean = " + str(np.mean(end_to_end_delays)) + "\n" + "Standard Deviation = " + str(np.std(end_to_end_delays)) + "\n"
    f = open(exp_name + ".txt", "w+")# Write results to the file
    f.write(out)
    f.close()
    print(out)
    termination_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # Say r3 that "YOU SHALL DIE IN PEACE MY BROTHER!" and r3 directs this message to d
    termination_socket.sendto("!".encode(), ("10.10.3.2", 7070))

main()
