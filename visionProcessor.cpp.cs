#include "CameraServer.h"
#include "cscore.h"
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


int visionProcessor()
{
    cs::UsbCamera camera = CameraServer::GetInstance()->StartAutomaticCapture();
    camera.SetResolution(640, 480);
    cs::CvSink cvSink = CameraServer::GetInstance()->GetVideo();
    cs::CvSource outputStreamStd = CameraServer::GetInstance()->PutVideo("Gray", 640, 480);
    extern Mat global_img;
    while(true) {
        outputStreamStd.PutFrame(global_img);
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }
    return 0;
}
