import socket

# Define the server address and port
HOST = '127.0.0.1'  # Localhost
PORT = 2222         # Port number (make sure it's the same as the server)

# Create a socket object
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

client_socket.connect((HOST, PORT))
print(f"Connected to {HOST} on port {PORT}")

while True:
    # Connect to the server
    # Get a message from the user
    message = input("Enter a message to send to the server: ")

    # Send the message to the server
    client_socket.sendall(message.encode('utf-8'))
    print(f"Message sent: {message}")

    # Wait for the server's response
    response = client_socket.recv(1024)  # Receive up to 1024 bytes
    print(f"Server response: {response.decode('utf-8')}")
