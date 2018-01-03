#include "RobotVision2017.h"

using namespace cv;

int webCamStream(string imageFileDirectory, int cam_number)
{
    int counter = 0;
    Mat cam_image;
    int FILE_NUM = 0;
    int image_save_freq = 10;
    
    VideoCapture cap(cam_number);

    std::string orig_image_file_name;

    while (true)
    {
        counter++;

        cap >> cam_image;
        if (cam_image.empty())
        {
            fprintf(stderr, "Failed to get frame\n");
        }

        // Save the frame into a file
        orig_image_file_name = imageFileDirectory + "/" + "camImage" + IntToString(FILE_NUM) + ".jpg";

        if (counter % image_save_freq == 0)
        {
            imwrite(orig_image_file_name, cam_image);
            FILE_NUM++;
            if (FILE_NUM >= MAX_IMAGES)
                FILE_NUM = 0;
        }

        processImage(cam_image, imageFileDirectory);

        if (waitKey(30) >= 0)
            break;
    }

    return (0);
}
