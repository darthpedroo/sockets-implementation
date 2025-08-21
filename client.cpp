#include <iostream>
#include <sys/socket.h> // Para sockets en general
#include <sys/un.h> // Para los sockets UNIX 
#include <unistd.h>
#include <thread>
#include <vector>
using namespace std;

int MAX_CLIENTS = 5;
int MAX_MSG_SIZE = 1024;
vector<thread>thread_pool;

int handle_write(int client_fd){
   while(1){
    string message;
    getline(cin, message);
    ssize_t bytes_sent = send(client_fd, message.c_str(), strlen(message.c_str()), 0);
    if (bytes_sent == -1) {
        cerr << "Error al enviar datos al socket" << endl;
        return 1;
    }
   }
}

int handle_recieve(int client_fd){
    while(1){
    char buffer[MAX_MSG_SIZE];
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes_received == -1) {
        cerr << "Error al recibir datos del socket" << endl;
        return 1;
    }
    cout << "Respuesta del servidor: " << string(buffer, bytes_received) << endl;
    }
}

// Ejemplo con UNIX sockets
int main(){
    // 1. Creamos el socket
    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        cerr << "Error al crear el socket" << endl;
        return 1;
    }

    // 2. Conectamos al servidor
    sockaddr_un address;
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, "/tmp/socket-example");

    int connect_result = connect(client_fd, (sockaddr*)&address, sizeof(address));
    if (connect_result == -1) {
        cerr << "Error al conectar al servidor" << endl;
        return 1;
    }
    thread th_write(handle_write,client_fd);
    thread th_read(handle_recieve, client_fd);

    if(th_write.joinable()) th_write.join();
    if(th_read.joinable()) th_read.join();

    close(client_fd);
    return 0;
}