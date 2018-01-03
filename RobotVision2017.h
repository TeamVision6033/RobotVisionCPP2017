/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   RobotVision2017.h
 * Author: ubuntu
 *
 * Created on February 4, 2017, 5:42 PM
 */

#ifndef ROBOTVISION2017_H
#define ROBOTVISION2017_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <thread>
#include "RTIMULib.h"

using namespace cv;
using namespace std;

const string ORIG_IMAGE_WIN = "Original";
const string CONTOUR_WIN = "Contours";
const string PROCESSED_IMAGE_WIN = "Processed";

#ifdef __ARM_FP
const string SD_CARD_DEV = "/dev/mmcblk1p1";
#else
const string SD_CARD_DEV = "/dev/mmcblk0p1";
#endif


const int MAX_IMAGES = 720;
const int BRIGHT_ADJUST = -20;
const int max_thresh = 255;
const int scale = 1;

const double ASPECT_RATIO_MIN = .300;
const double ASPECT_RATIO_MAX = .700;

struct nt_table_data {
    long long int seq_no;
    double steer;
    double distance;
    double angle;
    long long int data_seq_no;
    double heading = -999;
    long long int good_images = 0;
};

bool compareContourAreas(std::vector<cv::Point>, std::vector<cv::Point>);
int webCamStream(string, int);
int imageFileSource(string);
int processImage(Mat, string);
std::string IntToString(int number);
int visionProcessor();
std::string findSDCard();
int imuData();
int networkTableWrite();


#endif /* ROBOTVISION2017_H */

