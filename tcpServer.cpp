#include <iostream>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "tcpServer.hpp"
#include <sys/socket.h>
#include <string>

tcpServer::tcpServer(unsigned int port, clients c):
    port(port),
    c(c)
{
    for(int i = 0; i < c.maxClients; i++){
        c.client[i] = 0;
    }
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        std::cout << " error creating socket\n";
       exit(1); 
    }
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

    memset((char *) &serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if(bind(sock, (struct sockaddr *) &serv_addr, 
                sizeof(serv_addr)) <0){
        std::cout << "no bind\n";
        exit(1);   
    }

}

void tcpServer::listenNewClient(){
    fd_set readfds;

    listen(sock,3);
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);
    addrlen = sizeof(serv_addr);
    max_sd = sock;

    for(int i = 0; i < c.maxClients; i++){
        sd = c.client[i];
        if (sd > 0){
            FD_SET(sd, &readfds);
        }
        if (sd > max_sd){
            max_sd = sd;
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0)&& (errno!=EINTR)){
            std::cout << "select error\n";
        }
        if(FD_ISSET(sock, &readfds)){
            nSock = accept(sock, (struct sockaddr *)&serv_addr, (socklen_t*)&addrlen);
            if(nSock < 0){
                std::cout << "accept";
                exit(1);
            }

            for(int i = 0; i < c.maxClients; i++){
                if(c.client[i] == 0){
                    c.client[i] = nSock;
                    break;
                }
            }
        
        }
    
    }
//    clilen = sizeof(cli_addr);
/*    newSock = accept(sock, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
    if(newSock < 0) {
        std::cout << "no accept\n";
    } else {
        std::cout << "new connection!\n";
    }*/
}

void tcpServer::recieve(){
    memset(buffer,'0', 256);
    n = read(newSock, buffer, 255);
    if (n <= 0){
        std::cout << "no msg\n";
    } else {
        std::string s(buffer);
        std::string subs = s.substr(0, n);
        std::cout << subs << "\n";

    }
}

