import socket 

HOST = '127.0.0.1'
PORT = 7979

buffer = 5000

def recv_msg(sock):
    data = bytearray()
    msg = ''
    while not msg:
        recvd = sock.recv(buffer)
        if not recvd:
            raise ConnectionError()
        data += recvd
        msg = data.decode('utf-8')

    return msg


if __name__ == '__main__' :
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((HOST,PORT))
        print('Connected to: {} {}'.format(HOST, PORT))
    except ConnectionError:
        print('Error in socket')


    try:
        data = "client"
        data = data.encode('utf-8')
        sock.sendall(data)
    except  Exception as ex:
        print(ex)

    
    try:
        msg = recv_msg(sock)
        print('Received date {}'.format(msg))
    except ConnectionError:
        print('Recv msg error')
        sys.exit(1)

    print('Closing connection')


