import socket
from struct import *

# Prepare the UDP connection
UDP_IP = "192.168.42.100"
print "Receiver IP: ", UDP_IP
UDP_PORT = 50000
print "Port: ", UDP_PORT
sock = socket.socket(socket.AF_INET, # Internet
                    socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

# Continuously read from the UDP socket
while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    # The next line just chops a lot of floats from the received data chunk
    # Just activate and deactivate sensors in the Sensor UDP app to locate
    # the values you want
    print "received message: ",\
        "%1.4f" %unpack_from ('!f', data, 0),\
        "%1.4f" %unpack_from ('!f', data, 4),\
        "%1.4f" %unpack_from ('!f', data, 8),\
        "%1.4f" %unpack_from ('!f', data, 12),\
        "%1.4f" %unpack_from ('!f', data, 16),\
        "%1.4f" %unpack_from ('!f', data, 20),\
        "%1.4f" %unpack_from ('!f', data, 24),\
        "%1.4f" %unpack_from ('!f', data, 28),\
        "%1.4f" %unpack_from ('!f', data, 32),\
        "%1.4f" %unpack_from ('!f', data, 36),\
        "%1.4f" %unpack_from ('!f', data, 40),\
        "%1.4f" %unpack_from ('!f', data, 44),\
        "%1.4f" %unpack_from ('!f', data, 48),\
        "%1.4f" %unpack_from ('!f', data, 52),\
        "%1.4f" %unpack_from ('!f', data, 56),\
        "%1.4f" %unpack_from ('!f', data, 60),\
        "%1.4f" %unpack_from ('!f', data, 64),\
        "%1.4f" %unpack_from ('!f', data, 68),\
        "%1.4f" %unpack_from ('!f', data, 72),\
        "%1.4f" %unpack_from ('!f', data, 76),\
        "%1.4f" %unpack_from ('!f', data, 80),\
        "%1.4f" %unpack_from ('!f', data, 84),\
        "%1.4f" %unpack_from ('!f', data, 88),\
        "%1.4f" %unpack_from ('!f', data, 92)