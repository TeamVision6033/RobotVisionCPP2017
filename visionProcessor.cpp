#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
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
#include <signal.h>

using namespace cv;

void *display(void *);

int visionProcessor()
{

    //--------------------------------------------------------
    //networking stuff: socket, bind, listen
    //-------------------------------------------------------
    signal(SIGPIPE,SIG_IGN);
    int localSocket,
            remoteSocket,
            port = 1190;

    struct sockaddr_in localAddr,
            remoteAddr;
    pthread_t thread_id;


    int addrLen = sizeof (struct sockaddr_in);
    localSocket = -1;

    while (localSocket == -1)
    {
        localSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (localSocket == -1)
        {
            perror("socket() call failed!!");
        } else
        {
            localAddr.sin_family = AF_INET;
            localAddr.sin_addr.s_addr = INADDR_ANY;
            localAddr.sin_port = htons(port);

            if (bind(localSocket, (struct sockaddr *) &localAddr, sizeof (localAddr)) < 0)
            {
                perror("Can't bind() socket");
                localSocket = -1;
            }
        }
        if (localSocket == -1)
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    //Listening
    listen(localSocket, 3);

    std::cout << "Waiting for connections...\n"
            << "Server Port:" << port << std::endl;

    //accept connection from an incoming client
    while (1)
    {
        remoteSocket = accept(localSocket, (struct sockaddr *) &remoteAddr, (socklen_t*) & addrLen);

        if (remoteSocket < 0)
        {
            perror("accept failed!");
            exit(1);
        }
        std::cout << "Connection accepted" << std::endl;
        pthread_create(&thread_id, NULL, display, &remoteSocket);
    }
    return 0;
}

void *display(void *ptr)
{
    int socket = *(int *) ptr;

    extern Mat global_img;
    int bytes = 0;

    while (global_img.empty())
    {
        sleep(1);
    }

    std::string header_str = "HTTP/1.0 200 OK\r\nServer: 192.168.2.22:4097\r\n" \
                             "Cache-Control: no-cache\r\n" \
                             "Cache-Control: private\r\n" \
                             "Content-Type: multipart/x-mixed-replace;boundary=--boundary\r\n\r\n";
    if ((bytes = send(socket, header_str.c_str(), strlen(header_str.c_str()), 0)) < 0)
    {
        std::cerr << "bytes = " << bytes << std::endl;
    }

    while (1)
    {
        // Cloning the image here caused the server to
        // crash every after several seconds of operation
        //Mat img = global_img.clone();
        Mat img = global_img;
        std::vector<uchar> buff(250000);

        int params[3] = {0};
        params[0] = CV_IMWRITE_JPEG_QUALITY;
        params[1] = 100;

        imencode(".jpg", img, buff, std::vector<int>(params, params + 2));

        std::string content(buff.begin(), buff.end());
        std::string hdr = "--boundary\r\nContent-Type: image/jpeg\r\n" \
                          "Content-Length: " + std::to_string(content.length()) + "\r\n\r\n";

        if ((bytes = send(socket, hdr.c_str(), strlen(hdr.c_str()), 0)) < 0)
        {
            std::cerr << "bytes = " << bytes << std::endl;
            break;
        }

        if ((bytes = send(socket, buff.data(), buff.size(), 0)) < 0)
        {
            std::cerr << "bytes = " << bytes << std::endl;
            break;
        }
        std::string img_end = "\r\n";
        if ((bytes = send(socket, img_end.c_str(), strlen(img_end.c_str()), 0)) < 0)
        {
            std::cerr << "bytes = " << bytes << std::endl;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

}
