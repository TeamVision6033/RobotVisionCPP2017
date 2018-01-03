#include "opencv2/opencv.hpp"
#include <iostream>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h> 
#include <string.h>
#include <thread>
#include <chrono>
#include "RobotVision2017.h"
#include <signal.h>

using namespace cv;

void *send_data(void *);

int dataServer() {

    //--------------------------------------------------------
    //networking stuff: socket, bind, listen
    //--------------------------------------------------------
    signal(SIGPIPE,SIG_IGN);
    int localSocket,
            remoteSocket,
            port = 1189;

    struct sockaddr_in localAddr,
            remoteAddr;
    pthread_t thread_id;


    int addrLen = sizeof (struct sockaddr_in);

    localSocket = -1;

    while (localSocket == -1) {
        localSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (localSocket == -1) {
            perror("socket() call failed!!");
        } else {
            localAddr.sin_family = AF_INET;
            localAddr.sin_addr.s_addr = INADDR_ANY;
            localAddr.sin_port = htons(port);

            if (bind(localSocket, (struct sockaddr *) &localAddr, sizeof (localAddr)) < 0) {
                perror("Can't bind() socket");
                localSocket = -1;
            }
        }
        if (localSocket == -1)
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    //Listening
    listen(localSocket, 3);

    std::cout << "Waiting for data connections...\n"
            << "Server Port:" << port << std::endl;

    //accept connection from an incoming client
    while (1) {
        remoteSocket = accept(localSocket, (struct sockaddr *) &remoteAddr, (socklen_t*) & addrLen);

        if (remoteSocket < 0) {
            perror("accept failed!");
            exit(1);
        }
        std::cout << "Data Connection accepted" << std::endl;
        pthread_create(&thread_id, NULL, send_data, &remoteSocket);
    }
    return 0;
}

void *send_data(void *ptr) {
    int socket = *(int *) ptr;

    int bytes = 0;

    while (1) {
        extern nt_table_data table_data;
        char send_str[256];

        sprintf(send_str, "%Ld,%Ld,%.3f,%.1f,%.1f,%.3f\n", table_data.seq_no, table_data.seq_no - table_data.data_seq_no, table_data.steer, table_data.distance, table_data.angle, table_data.heading);

        if ((bytes = send(socket, send_str, strlen(send_str), 0)) < 0) {
            std::cerr << "bytes = " << bytes << std::endl;
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

}
