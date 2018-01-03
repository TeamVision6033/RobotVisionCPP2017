cmake_minimum_required(VERSION 2.8)
project( RobotVision2017 )
file(GLOB SOURCES "./*.cpp" "IMUDrivers/*.cpp")
file(GLOB HEADERS "./*.h" "IMUDrivers/*.h")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} include . /home/ubuntu/wpi/include)
add_executable (robotvision2017 ${SOURCES})
target_link_libraries( robotvision2017 ${OpenCV_LIBS} /home/ubuntu/wpi/lib/libntcore.so -lpthread )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CUDA_USE_STATIC_CUDA_RUNTIME OFF)
#g++ main.cpp -o output -Wall -std=c++11  -lstdc++ -pthread  -Iinclude/

SET(LIBRTIMU_SRCS
    RTFusion.cpp
    RTFusionRTQF.cpp
    RTMath.cpp
    RTFusionKalman4.cpp
    RTIMUAccelCal.cpp
    RTIMUHal.cpp
    RTIMUMagCal.cpp
    RTIMUSettings.cpp
    IMUDrivers/RTIMU.cpp
    IMUDrivers/RTIMUGD20M303DLHC.cpp
    IMUDrivers/RTIMUGD20HM303DLHC.cpp
    IMUDrivers/RTIMUGD20HM303D.cpp
    IMUDrivers/RTIMULSM9DS0.cpp
    IMUDrivers/RTIMULSM9DS1.cpp
    IMUDrivers/RTIMUMPU9150.cpp
    IMUDrivers/RTIMUMPU9250.cpp
    IMUDrivers/RTIMUBMX055.cpp
    IMUDrivers/RTIMUBNO055.cpp
    IMUDrivers/RTIMUNull.cpp
    IMUDrivers/RTPressure.cpp
    IMUDrivers/RTPressureBMP180.cpp
    IMUDrivers/RTPressureLPS25H.cpp
    IMUDrivers/RTPressureMS5611.cpp
    IMUDrivers/RTPressureMS5637.cpp
    IMUDrivers/RTHumidity.cpp
    IMUDrivers/RTHumidityHTS221.cpp
    IMUDrivers/RTHumidityHTU21D.cpp
)

    ADD_LIBRARY(RTIMULib SHARED ${LIBRTIMU_SRCS})
    SET_PROPERTY(TARGET RTIMULib PROPERTY VERSION ${RTIMULIB_VERSION})
    SET_PROPERTY(TARGET RTIMULib PROPERTY SOVERSION ${RTIMULIB_VERSION_MAJOR})
    #INSTALL(TARGETS RTIMULib DESTINATION lib)
    #INSTALL(DIRECTORY . DESTINATION include FILES_MATCHING PATTERN "*.h")
