import socket
import threading
import struct
import sys
import time
import hashlib
import statistics as st
import math

if sys.argv[1] == "exp1":# destination ips for exp1
    dst_ips = ["10.10.3.2", "10.10.3.2"]
elif sys.argv[1] == "exp2":# destination ips for exp2
    dst_ips = ["10.10.1.2", "10.10.2.1"]
else:# else terminate
    sys.exit()

payload_size = 512# Fixed payload size
N = 5000000//payload_size + 1# Fixed size of 512 byte chunks
window_size = 16# Window size, no spesific reason behind choosing 16, it could have been anything
byte_chunks = ["".encode()]*(N + 1)# Fixed array of byte chunks

# Calculates checksum by using md5 sum implemented in hashlib
def calculate_checksum(payload):
    return hashlib.md5(payload).hexdigest().encode()

# Given sequence number, acknowledgement number, and payload, creates a packet
def create_packet(seq_n, ack_n, payload="".encode()):
    checksum = calculate_checksum(payload)
    length = 38 + len(payload)
    return struct.pack("!HHH", seq_n, ack_n, length) + checksum + payload

# Given a packet extract sequence number, acknowledgement number, length, and payload of the packet
def extract_packet(packet):
    return (*struct.unpack("!HHH", packet[:6]), packet[6:38], packet[38:])

# Given a file name divides content of that file into 512 byte chunks
def divide_into_byte_chunks(file_name):
    global payload_size, N, byte_chunks
    f = open(file_name, "rb")
    content = f.read()
    f.close()
    for i in range(N):
        byte_chunks[i] = content[i*payload_size : (i + 1)*payload_size]

# Function traversing byte_chunks list and sending payloads by following our-selective-repeat-like protocol
def file_sender():
    global dst_ips, payload_size, N, base, window_size, is_acked, is_sent, byte_chunks, timeout_interval, estimated_rtt, dev_rtt, timeout_interval, estimated_rtt, timeout_interval_lock, starts, is_timedout, is_timedout_lock, timeouts
    file_sender_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)# Create socket for sending payloads
    reached_end = False# base + window_size does not extend end of the file yet
    file_transfer_time_start = time.time()# Start file transfer time
    while not all(is_acked[:-1]):# While there are still packets that are not acked. last element of this list is special it is not a valid packet
        is_timedout_lock.acquire()# Acquire lock for timeout-related global variables
        if is_timedout:# if the link is down is_timedout boolean variable will be true
            is_timedout = False# reset flag
            for i in timeouts:# for all packets that are not sent due to the link being down
                key = i % 2# odd or even
                if dst_ips[key] != dst_ips[(key + 1) % 2]:# if destination ips are not swapped yet, copy live link's ips and timeout value related variables onto the dead links'
                    dst_ips[key] = dst_ips[(key + 1) % 2]
                    timeout_interval[key] = timeout_interval[(key + 1) % 2]
                    estimated_rtt[key] = estimated_rtt[(key + 1) % 2]
                    dev_rtt[key] = dev_rtt[(key + 1) % 2]
                threading.Thread(target=sender, daemon=True, args=(file_sender_socket, i)).start()# create a new sender thread for failed packet
            timeouts.clear()# flush failed packets list
        is_timedout_lock.release()# release lock
        if not reached_end:# if base + window_size did not reach to end of file yet, try to increment base
            for i in range(base, base + window_size):
                if is_acked[i]:
                    if base + window_size + 1 <= N:
                        base += 1
                    else:
                        reached_end = True
                        break
                else:
                    break
        for i in range(base, base + window_size):# for all packets in the window if they are not sent yet, send them
            if not is_sent[i]:
                threading.Thread(target=sender, daemon=True, args=(file_sender_socket, i)).start()
                is_sent[i] = True
    file_transfer_time_end = time.time()# file transfer finished, get end time
    sender(file_sender_socket, -1)# reliablely send a finish packet
    return file_transfer_time_end - file_transfer_time_start#return file transfer time

# Receives ACKs and marks them as received in the global list
def ack_receiver():
    global dst_ips, payload_size, N, base, window_size, is_acked, is_sent, byte_chunks, timeout_interval, estimated_rtt, dev_rtt, timeout_interval, estimated_rtt, timeout_interval_lock, starts, is_timedout, is_timedout_lock, timeouts
    ack_receiver_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)# Creates ack sender socket
    ack_receiver_socket.bind(("0.0.0.0", 8080))# Bind it to all ips. It is safe since given topology is well-defined and strict
    while not all(is_acked):# While there are packets that are not acked yet
        packet, _ = ack_receiver_socket.recvfrom(1024)# receive packet
        end = time.time()# get end time for round trip time
        seq_n, ack_n, _, _, _ = extract_packet(packet)
        i = ack_n - 1# index of ack
        is_acked[ack_n - 1] = True# mark as acked
        key = i % 2 if dst_ips[0] != dst_ips[1] else 0# if both links are not alive, pick the link with index 0 else pick proper index
        sample_rtt = end - starts[i]# sample rtt
        if estimated_rtt[key] == None:
            estimated_rtt[key] = sample_rtt
        estimated_rtt[key] = 0.875*estimated_rtt[key] + 0.125*sample_rtt# alpha = 0.125
        temp = abs(sample_rtt - estimated_rtt[key])
        if dev_rtt[key] == None:
            dev_rtt[key] = temp
        dev_rtt[key] = 0.75*dev_rtt[key] + 0.25*abs(temp)# beta = 0.25
        timeout_interval_lock[key].acquire()
        timeout_interval[key] = estimated_rtt[key] + 4*dev_rtt[key]# new timeout interval for the link with the key
        timeout_interval_lock[key].release()

# Function that is responsible from reliabley tranfering a given packet
def sender(file_sender_socket, i):
    global dst_ips, payload_size, N, base, window_size, is_acked, is_sent, byte_chunks, timeout_interval, estimated_rtt, dev_rtt, timeout_interval, estimated_rtt, timeout_interval_lock, starts, is_timedout, is_timedout_lock, timeouts
    send_count = 0# how many times packet is sent to indicate link's down/up status
    key = i % 2 if dst_ips[0] != dst_ips[1] else 0# calculate key, that is index
    f = False# send_count did not exceed 1000 yet
    while not is_acked[i]:# while packet with given index is not acked yet
        is_timedout_lock.acquire()# Acquire lock for timeout checks
        if send_count >= 1000:# if send_count exceeeds 100
            is_timedout = True# set flag
            timeouts.append(i)# append index of packet to the failed packets list
            f = True# set flag
        is_timedout_lock.release()# Release lock
        if f:# if flag is set, return
            return
        timeout_interval_lock[key].acquire()# Acquire key for fetching timeout value
        timeout_value = timeout_interval[key]# store timeout value
        timeout_interval_lock[key].release()# Release lock
        starts[i] = time.time()# save start time of packet for rtt
        file_sender_socket.sendto(create_packet(i + 1, 0, byte_chunks[i]), (dst_ips[key], 8080))# send packet
        time.sleep(timeout_value)# sleep until timeout value, after wake up while loop checks if the ack is received
        send_count += 1# increment send_count

# Initialize all globals
def init():
    global dst_ips, payload_size, N, base, window_size, is_acked, is_sent, byte_chunks, timeout_interval, estimated_rtt, dev_rtt, timeout_interval, estimated_rtt, timeout_interval_lock, starts, is_timedout, is_timedout_lock, timeouts
    base = 0
    is_acked = [False]*(N + 1)
    is_sent = [False]*N
    timeout_interval = [0.05, 0.05]
    estimated_rtt = [None, None]
    dev_rtt = [None, None]
    timeout_interval_lock = [threading.Lock(), threading.Lock()]
    starts = [None]*(N + 1)
    is_timedout = False
    is_timedout_lock = threading.Lock()
    timeouts = []

# Return file transfer time for a single file transfer operation
def sample():
    init()# initialize globals
    threading.Thread(target=ack_receiver, daemon=True).start()# create an ack receiver thread
    return file_sender()# send file

# main function
def main():
    divide_into_byte_chunks(sys.argv[2])# divide file into byte chunks
    ftts = []# file transfer times of experiments
    error = 1# initial margin of error
    ftts.append(sample())# perform first file transfer
    threshold = float(sys.argv[3])# get threshold for margin of error from terminal arguments
    print(ftts[-1])# print first sample
    sys.stdout.flush()# fluck stdout
    if threshold > 0:# if a valid threshold is given, then perform multiple experiments until given margin of error is achieved
        while error >= threshold:# until margin of error is achieved
            ftts.append(sample())# get a new sample
            error = 1.96*st.pstdev(ftts)/math.sqrt(len(ftts))# calculate new margin of error
            print(len(ftts), error, ftts[-1])# print information
            sys.stdout.flush()# flush stdout
        print("size = ", len(ftts))# report results
        print("mean = ", st.mean(ftts))
        print("standard deviation = ", st.pstdev(ftts))
        print("margin of error = ", error)
        print("median = ", st.median(ftts))
        sys.stdout.flush()

main()
