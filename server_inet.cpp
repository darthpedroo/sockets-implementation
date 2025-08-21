#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <vector>
using namespace std;

int MAX_CLIENTS = 5;
int MAX_MSG_SIZE = 1024;

vector<thread>thread_pool;
vector<int>client_pool;

int handle_client(int client_fd){

    char buffer[MAX_MSG_SIZE];
    while(1){
        
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received < 0) {
            cerr << "Error al recibir datos del socket" << endl;
            return 1;
        }
        
        cout << "Mensaje recibido del cliente: " << string(buffer, bytes_received) << endl;

        for (int i = 0; i < client_pool.size(); i++){
            cout << client_pool[i] << endl;

            if (client_pool[i] != client_fd){
                ssize_t bytes_sent = send(client_pool[i], buffer, bytes_received, 0);
                if (bytes_sent < 0) {
                    cerr << "Error al enviar datos al socket" << endl;
                    return 1;
                }
            }
        }
    }
    close(client_fd);
}

int main(){
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        cerr << "Error al crear el socket" << endl;
        return 1;
    }
    
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int bind_result = ::bind(server_socket, (struct sockaddr*)& serverAddress,
sizeof(serverAddress));

    if (bind_result < 0) {
        cerr << "Error al vincular el socket" << endl;
        return 1;
    }

    int listen_result = listen(server_socket, MAX_CLIENTS); // 5 es el número máximo de conexiones en cola
    if (listen_result < 0) {
        cerr << "Error al escuchar en el socket" << endl;
        return 1;
    } else {
        cout << "Escuchando en el socket..." << endl;
    }

    while(1){
        // 4. Aceptamos conexiones entrantes
        int client_fd = accept(server_socket, nullptr, nullptr);
        client_pool.push_back(client_fd);

        if (client_fd < 0) {
            cerr << "Error al aceptar la conexión" << endl;
            return 1;
        }
        thread th = thread(handle_client,client_fd);
        thread_pool.push_back(move(th));
        
        // 7. Cerramos los sockets
        //close(client_fd);
    }
    close(server_socket);
    for (auto& th: thread_pool) {
            if (th.joinable()) { th.join(); }
        }    
    return 0;
}