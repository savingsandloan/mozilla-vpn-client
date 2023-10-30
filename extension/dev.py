import socket
import threading
import sys


# Predefined list of messages
messages = [
    '{"t":"status"}', 
    '{"t":"activate"}', 
   ]

# Function to display a list of messages and allow the user to select one
def select_message():
    print("Select a message to send:")
    for i, message in enumerate(messages):
        print(f"{i + 1}. {message}")

    while True:
        try:
            choice = int(input("Enter the number of the message to send: "))
            if 1 <= choice <= len(messages):
                return messages[choice - 1]
            else:
                print("Invalid choice. Please enter a valid number.")
        except ValueError:
            print("Invalid input. Please enter a number.")



# Function to read from the terminal and send data to the server
def send_data_to_server(client_socket):
    while True:
        message = select_message()
        client_socket.send(len(message).to_bytes(4, 'little') )
        client_socket.send(message.encode())

# Function to receive data from the server and write it to the terminal
def receive_data_from_server(client_socket):
    while True:
        data = client_socket.recv(1024)

        if not data:
            break
        print("\n=========== \n")
        print(data[4:].decode())
        print("\n=========== \n")

def main():
    # Create a TCP socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Get server host and port from command-line arguments
    if len(sys.argv) != 3:
        print("Usage: python client.py <server_host> <server_port>")
        sys.exit(1)

    server_host = sys.argv[1]
    server_port = int(sys.argv[2])

    try:
        # Connect to the server
        client_socket.connect((server_host, server_port))

        # Create two threads for sending and receiving data
        send_thread = threading.Thread(target=send_data_to_server, args=(client_socket,))
        receive_thread = threading.Thread(target=receive_data_from_server, args=(client_socket,))

        # Start the threads
        send_thread.start()
        receive_thread.start()

        send_thread.join()
        receive_thread.join()
    except Exception as e:
        print(f"Error: {e}")
    finally:
        # Close the socket when done
        client_socket.close()

if __name__ == "__main__":
    main()
