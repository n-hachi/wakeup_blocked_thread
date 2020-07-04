#/usr/bin/env python3
import socket
import sys


msg = "message"
if len(sys.argv) > 1:
    msg = sys.argv[1]
print("send : " + msg)

ip = "0.0.0.0"
port = 10000
addr = (ip, port)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.sendto(msg.encode(), addr)
