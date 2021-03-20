from socket import socket
import json

def send(sock, code, params):
    buffer = chr(code).encode('utf-8')

    json_bytes = json.dumps(params,ensure_ascii=False).encode('utf-8')
    json_length = len(json_bytes)
    json_length_bytes = b''
    json_length_bytes += chr(0xFF & json_length).encode('utf-8')
    json_length_bytes += chr(0xFF & (json_length << 8)).encode('utf-8')
    json_length_bytes += chr(0xFF & (json_length << 16)).encode('utf-8')
    json_length_bytes += chr(0xFF & (json_length << 24)).encode('utf-8')
    buffer += json_length_bytes
    buffer += json_bytes
    sock.send(buffer)

def get(sock):
    code = ord(sock.recv(1))
    json_length_bytes = sock.recv(4)
    json_length = int.from_bytes(json_length_bytes, byteorder='little')
    params = json.loads(sock.recv(json_length).decode('utf-8'))
    return (code, params)


if __name__ == "__main__":
    s = socket()
    s.connect(('127.0.0.1', 4444))
    print('Connected!')
