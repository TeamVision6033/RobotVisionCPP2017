#include <iostream>
#include <chrono>
#include <cstdio>
#include <thread>
#include "ntcore.h"
#include "RobotVision2017.h"

#include "networktables/NetworkTable.h"

using namespace std;
using std::shared_ptr;

int networkTableWrite() {
    NetworkTable::SetClientMode();
    NetworkTable::SetTeam(6033);

    int sleepMilliseconds = 1000;

    shared_ptr<NetworkTable> table = NetworkTable::GetTable("datatable");
    std::this_thread::sleep_for(std::chrono::seconds(5));

    while (1) {
    	extern nt_table_data table_data;
        if (table != nullptr) {
            try {
                table->PutNumber("VisionSeqNo", table_data.seq_no);
                table->PutNumber("VisionAge", table_data.seq_no - table_data.data_seq_no);
                table->PutNumber("VisionSteer", table_data.steer);
                table->PutNumber("VisionDistance", table_data.distance);
                table->PutNumber("VisionAngle", table_data.angle);
                table->PutNumber("VisionHeading", table_data.heading);
                sleepMilliseconds = 50;
            } catch (...) {
                cerr << "Caught exception in networkTableWrite\n";
            }
        }
        else
        	table = NetworkTable::GetTable("datatable");
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepMilliseconds));
    }
    return (0);
}
