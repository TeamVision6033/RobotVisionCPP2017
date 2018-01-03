#include "RobotVision2017.h"
#include <sys/stat.h>

using namespace std;

bool SHOW_ORIG_IMAGE = true;
bool SHOW_PROCESSED_IMAGE = false;
bool SHOW_CONTOUR_IMAGE = false;

string light_color = "blue";

Mat global_img;
nt_table_data table_data;

int main(int argc, char** argv)
{

    int test;
    int cam_number = 0;
    int x;

    std::string imageFileDirectory;

    struct stat sb;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s file|cam <cam_number=0>| <all|none>\n", argv[0]);
        return (-1);
    }
    if (argc > 2 && std::string(argv[2]) == "1")
        cam_number = 1;

    for (x = 1; x < argc; x++)
    {
        if (std::string(argv[x]) == "all")
        {
            SHOW_ORIG_IMAGE = true;
            SHOW_PROCESSED_IMAGE = true;
            SHOW_CONTOUR_IMAGE = true;
        }
        if (std::string(argv[x]) == "none")
        {
            SHOW_ORIG_IMAGE = false;
            SHOW_PROCESSED_IMAGE = false;
            SHOW_CONTOUR_IMAGE = false;
        }
    }


    std::string sd_card_dir = findSDCard();
    if (sd_card_dir.length() > 1)
    {
        if (!(stat(sd_card_dir.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)))
        {
            fprintf(stderr, "SD Card not found\n");
            return (-2);
        }
        imageFileDirectory = sd_card_dir + "/images/";
        if (!(stat(imageFileDirectory.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)))
        {
            mkdir(imageFileDirectory.c_str(), ACCESSPERMS);
        }
    }

    if (!(stat(imageFileDirectory.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)))
    {
        fprintf(stderr, "Correct SD card must be inserted\n");
        return (-2);
    }

    #ifdef __ARM_FP
        thread imu_thread(imuData);
    #endif

    thread cs_thread(visionProcessor);
    thread ntWriteThread(networkTableWrite);

    if (SHOW_ORIG_IMAGE)
        namedWindow(ORIG_IMAGE_WIN, WINDOW_AUTOSIZE);
    if (SHOW_PROCESSED_IMAGE)
        namedWindow(PROCESSED_IMAGE_WIN, WINDOW_AUTOSIZE);
    if (SHOW_CONTOUR_IMAGE)
        namedWindow(CONTOUR_WIN, CV_WINDOW_AUTOSIZE);



    if (std::string(argv[1]) == "cam")
        test = webCamStream(imageFileDirectory, cam_number);
    else if (std::string(argv[1]) == "file")
        test = imageFileSource(imageFileDirectory);
    else
    {
        fprintf(stderr, "Usage: %s file|cam <cam_number=0>\n", argv[0]);
        return (-3);
    }
     fprintf(stderr, "Finished\n");
    return 0;
}

std::string IntToString(int number)
{
    std::ostringstream oss;

    // Works just like cout
    oss << number;

    // Return the underlying string
    return oss.str();
}

