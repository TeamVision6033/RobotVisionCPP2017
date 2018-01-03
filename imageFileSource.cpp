#include "RobotVision2017.h"
#include<fstream>
#include<dirent.h>

void listFile()
{
    DIR *pDIR;
    struct dirent *entry;
    if (pDIR = opendir("./data/item"))
    {
        while (entry = readdir(pDIR))
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                cout << entry->d_name << "\n";
        }
        closedir(pDIR);
    }
}

int imageFileSource(string imageFileDirectory)
{
    Mat file_image;
    string image_file_name;

    DIR *pDIR;
    struct dirent *entry;
    if (pDIR = opendir(imageFileDirectory.c_str()))
    {
        while (entry = readdir(pDIR))
        {

            string file_name = entry->d_name;
            if (file_name.find("camImage") == 0)
            {
                image_file_name = imageFileDirectory + file_name;
                std::cout << image_file_name + "\n";
                file_image = imread(image_file_name, 1);
                try
                {
                    processImage(file_image, imageFileDirectory);
                }
                catch (...)
                {
                    fprintf(stderr, "Failed to process image\n");
                }
                waitKey(90);
            }
        }
        closedir(pDIR);
    }
    waitKey(1000);
    return (0);
}

