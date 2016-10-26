
cmake_minimum_required(VERSION 3.5)

include_directories(.. ../External)
add_definitions(-DUSE_MBED -D_LINUX)

set(CMAKE_CXX_STANDARD 14)


add_library(StormData STATIC
            StormDataChangeNotifier.cpp  
            StormDataChangePacket.cpp  
            StormDataChangePacketHelpers.cpp  
            StormDataJsonUtil.cpp)



