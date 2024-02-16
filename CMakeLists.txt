cmake_minimum_required(VERSION 3.26)

project(Roboter_Steuerung_jetson_nano)

set(CMAKE_CXX_STANDARD 17)

# Add CMakeLists.txt
#add_subdirectory(external/tensorflow/tensorflow/lite) # from tensorflow-lite
#add_subdirectory(external/opencv) # from OpenCV

# Include the needed directories
include_directories(external/tensorflow)
include_directories(cmake-build-debug)
include_directories(cmake-build-debug/flatbuffers/include)

file(GLOB modules external/opencv/modules/*) # include every OpenCV module
FOREACH(module ${modules})
    include_directories(${module}/include)
ENDFOREACH()

# Add sources
add_executable(Roboter_Steuerung_jetson_nano src/main.cpp
        include/tensorflow/Tfml.hpp
        src/tensorflow/Tfml.cpp
        include/video/Vstream.hpp
        include/serial/Aserial.hpp
        src/serial/Aserial.cpp)

# link the libraries so that they build
#target_link_libraries(Roboter_Steuerung_jetson_nano OpenCV)
#target_link_libraries(Roboter_Steuerung_jetson_nano tensorflow-lite)

# add the include for the projects cpp files
target_include_directories(Roboter_Steuerung_jetson_nano PUBLIC
        include
)






