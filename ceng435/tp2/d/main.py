import socket
import threading
import struct
import hashlib
import sys

dst_ips = {"10.10.3.2":"10.10.7.2", "10.10.1.2":"10.10.8.1", "10.10.2.1":"10.10.8.2"}# destination ip mappings. for a (key,value) pair send ack to value ip if packet comes from key ip

payload_size = 512# Fixed payload size
N = 5000000//payload_size + 1# Fixed size of 512 byte chunks

byte_chunks = [None]*N# Fixed array of byte chunks

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

# receives a file
def file_receiver():
    global dst_ips, payload_size, N, byte_chunks
    file_receiver_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)# create file receiver socket
    file_receiver_socket.bind(("0.0.0.0", 8080))# Bind it to all ips. It is safe since given topology is well-defined and strict
    ack_sender_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)# create ack receiver socket
    while True:
        packet, (ip, port) = file_receiver_socket.recvfrom(1024)# receive packet
        seq_n, ack_n, _, checksum, payload = extract_packet(packet)# extract packet
        if seq_n == 0:# if finish packet
            ack_sender_socket.sendto(create_packet(0, 0), (dst_ips[ip], port))# send finish ack and return
            return
        if checksum == calculate_checksum(payload):# if packet is not corrupted
            if byte_chunks[seq_n - 1] == None:# if byte chunk index is not filled yet
                byte_chunks[seq_n - 1] = payload# store payload in corresponding index
            ack_sender_socket.sendto(create_packet(0, seq_n), (dst_ips[ip], port))# send ack

# Writes received file into the output file
def write_to_file(file_name):
    global dst_ips, payload_size, N, byte_chunks
    f = open(file_name, "wb+")
    f.writelines(byte_chunks)
    f.close()

# receives file and writes it into output file
def sample():
    file_receiver()
    write_to_file(sys.argv[1])

# main function  
def main():
    repetition_count = int(sys.argv[2])# if repetition_count is negative, repeate infinitely, else repeat for repetition_count
    if repetition_count < 0:
    	while True:
    		sample()
    for i in range(repetition_count):
        sample()

main()
