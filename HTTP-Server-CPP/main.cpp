//
//  main.cpp
//  HTTP-Server-CPP
//
//  Created by Abhee Chaudhary on 03/11/24.
//

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sstream>

int main(int argc, const char * argv[]) {
    
    int server_fd, new_socket; // server_fd is the server socket , new_socket is the client socket
    struct sockaddr_in address;// defined in netinet/in.h and is used to store the server's IP
    int addrlen = sizeof(address); // length of the address
    
    // creates server socket :
    server_fd = socket(AF_INET, SOCK_STREAM, 0); // creates a new socket -> AF_INET : internet Domain(IPv4), SOCK_STREAM : Specifies a TCP socket, 0 : uses the default protocol (TCP when SOCK_STREAM is chosen)
    if (server_fd == 0) {
        std::cerr << "Socket Creation Failed\n";
        return -1;
    }
    
    //define server address :
    
    address.sin_family = AF_INET; // use IPv4
    address.sin_addr.s_addr = INADDR_ANY; // accept connections from any IP
    address.sin_port = htons(8080); // bind to port 8080 (converted to network byte order)
    /*
     sin_family : sets the address family for AF_INET to cpecify IPv4
     sin_addr.s_addr : sets the IP address . INADDR_ANY allows server to accept connections on any network interface
     s - set - in - incoming - addr - address
     sin_port : sets the port to 8080. htons() -> Host to Network Short : converts the port to network byte order
     */
    
    
    // Bind the socket to IP Address and Port :
    if (bind(server_fd , (struct sockaddr *)&address , sizeof(address))<0) {
        std::cerr << "Bind Failed\n";
        close(server_fd);
        return -1;
    }
     /*
      bind(server_fd , (struct sockaddr * ) &address , sizeof(address)) : binds the server socket(server_fd) to IP address and port defined in address.
      we cast address to a generic sockaddr type as address was sockaddr_in type as bind requires this format
      if binding fails we print err and socket closes
      */
    
    // listen for incoming connections :
    
    if (listen(server_fd, 3)<0) { // listen(server_fd) : marks the socket as passive that will listen for incomming connection requests, 3: is the backlog parameter for maximum number of queued connections before refusing new ones
        std::cerr << "Listen failed\n";
        close(server_fd);
        return -1;
    }
    
    // accept incoming connection
    std::cout << "Server is listening on port 8080\n";
    new_socket = accept(server_fd, (struct sockaddr * )&address, (socklen_t*)&addrlen); // waits for client to connect returns a new socket file descriptor (new_socket) that represents the connection to the client
    if(new_socket<0){
        std::cerr <<"Accept Failed\n";
        close(server_fd);
        return -1;
        
    }// after accepting connection lets parse incoming request
    
    // read and parse client request
    
    char buffer[1024] = {0};
    read(new_socket, buffer, 1024);
    std::cout << "Request:\n" << buffer << "\n";
    // extract http method and path
    
    std :: string request(buffer);
    std:: string method,path;
    
    std::istringstream request_stream(request);
    request_stream >> method >> path;
    std::cout << "Method : " << method << " , Path : " << path << "\n";
    
    // implement basic routing
    
    std::string response;

    if (method == "GET" && path == "/") {
        response = "HTTP/1.1 200 OK\r\n"
                   "Content-Type: text/plain\r\n"
                   "Content-Length: 19\r\n"
                   "\r\n"
                   "Welcome to the server!";
    } else if (method == "GET" && path == "/hello") {
        response = "HTTP/1.1 200 OK\r\n"
                   "Content-Type: text/plain\r\n"
                   "Content-Length: 13\r\n"
                   "\r\n"
                   "Hello, World!";
    } else {
        response = "HTTP/1.1 404 Not Found\r\n"
                   "Content-Type: text/plain\r\n"
                   "Content-Length: 9\r\n"
                   "\r\n"
                   "Not Found";
    }
    
    
    
    // send HTTP response to the client
    
    // 8. Send an HTTP response using std::string
//    const std::string response = "HTTP/1.1 200 OK\r\n"
//                           "Content-Type: text/plain\r\n"
//                           "Content-Length: 13\r\n"   // Length of "Hello, World!"
//                           "\r\n"
//                           "Hello, World!";

    
    send(new_socket, response.c_str(), response.length(), 0); // convert std::string to const char* as it is needed in the send function
    std::cout << "Response sent : \n" << response << "\n";
    
    // closing the sockets :
    
    close(new_socket);
    close(server_fd);
    
    
    
    
    return 0;
    
}
