import socket 
import time
import sys
HOST = '127.0.0.1'
PORT = 7979

buffer = 50000

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

def send_msg(sock, msg):
    data = msg.encode('utf-8')
    sock.sendall(data)


def register(sockfd, username, password):
    
    send_msg(sockfd, username)
    time.sleep(0.3)
    send_msg(sockfd, password)
    msg = recv_msg(sockfd)

    return msg
    # try:
    #     msg = recv_msg(sock)
    #     print('Received date {}'.format(msg))
    # except ConnectionError:
    #     print('Recv msg error')
    #     sys.exit(1)

def login(sockfd, username, password):
    
    send_msg(sockfd, username)
    time.sleep(0.3)
    send_msg(sockfd, password)
    time.sleep(3)

    # try:
    #     msg = recv_msg(sock)
    #     print('Received date in login {}'.format(msg))
    #     send_msg(sockfd, "2")
    # except ConnectionError:
    #     print('Recv msg error')
    #     sys.exit(1)
    
    



if __name__ == '__main__' :
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((HOST,PORT))
        print('Connected to: {} {}'.format(HOST, PORT))
    except ConnectionError:
        print('Error in socket')


    try:
       send_msg(sock, "client")
    except  Exception as ex:
        print(ex)
    
    try:
        recv = recv_msg(sock)
        print(recv)
    except  Exception as ex:
        print(ex) 

    username = "aa"
    password = "aa" 
    ok = "ok"
    connected = False
    while True:
        if connected == False:
            print("1. Creare cont")
            print("2. Conectare")
            print("3. Adio")
            optiune = input("Optiune:")
            
            if optiune == "1":
                try:
                    send_msg(sock, "1")
                    # recv ok de la server
                    recv = recv_msg(sock)
                    recv = str(recv)
                    if recv:
                        msg = register(sock, username, password)
                    else:
                        print("server error")
                except Exception as ex:
                    print(ex)

            elif optiune == "2":
                try:
                    send_msg(sock, "2")
                    recv = recv_msg(sock)
                    print("de la server {}".format(recv))
                    if recv:
                        login(sock, username, password)
                        connected = True
                    else:
                        print("server error")
                except Exception as ex:
                    print(ex)

        elif connected == True:
            print("1. Adaugare date noi\n")
            print("2. Vizualizare date\n")
            print("3. Cauta o valoare specifica\n")
            print("4. Modificare date\n")
            print("5. Stergere TOATE datele\n")
            print("6. Stergere o pereche anume\n")
            print("7. Log out\n")
            print("8. Exit\n")
            optiune = input("Optiune:")

            if optiune == "1":
                try:
                    send_msg(sock, "1")
                    recv = recv_msg(sock)
                    if recv:
                        key = input("Introduceti numele secretului: \n")
                        send_msg(sock, key)
                        time.sleep(0.5)
                        value = input("Introduceti valoarea: \n")
                        send_msg(sock, value)
                        time.sleep(1)
                except Exception as ex:
                    print(ex)
            elif optiune == "2" :
                try:
                    send_msg(sock, "2")
                    recv = recv_msg(sock)
                    print("in getall {}".format(recv))
                    if recv:
                        msg = recv_msg(sock)
                        msg = recv_msg(sock)
                        
                        print("Print your data ----> {}".format(msg))
                except Exception as ex:
                    print(ex)
            




                



            
                
        

    # while True:
    #     try:
    #         username = "test"
    #         password = "123"
    #         option = "2"
    #         msg = recv_msg(sock)
    #         print('Received date in main {}'.format(msg))
    #         if msg == "ok":
    #             login(sock, option, username, password)
    #         else:
    #             break

    #     except ConnectionError:
    #         print('Recv msg error')
    #         sys.exit(1)

    #register(sock, "1", "dinpython", "dinpython")
    
    #login(sock, option, username, password)
    
    #send_msg(sock, option)