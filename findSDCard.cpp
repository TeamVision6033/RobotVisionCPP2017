#include <fstream>
#include <sstream>
#include "RobotVision2017.h"

std::string findSDCard() {

    std::ifstream infile("/proc/mounts");
    std::string line;
    while (std::getline(infile, line)) {
        if (line.substr(0, SD_CARD_DEV.length()) == SD_CARD_DEV)
        {
            std::string tmp = line.substr(line.find(' ') + 1);
            tmp = tmp.substr(0,tmp.find(' '));
            return tmp;
        }
    }
    infile.close();

    return "";
}
