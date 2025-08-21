#!/bin/bash
mkdir build
g++ server_inet.cpp -o build/server_inet
g++ client_inet.cpp -o build/client_inet
g++ server.cpp -o build/server
g++ client.cpp -o build/client