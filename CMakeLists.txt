cmake_minimum_required(VERSION 3.10)

project(Roboter_Steuerung_jetson_nano)

set(CMAKE_CXX_STANDARD 17)
set(TFLITE_ENABLE_GPU true)

set(TENSORFLOW_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/tensorflow")
set(TFLITE_SOURCE_DIR "${TENSORFLOW_SOURCE_DIR}/tensorflow/lite")

# Add CMakeLists.txt
add_subdirectory(${TFLITE_SOURCE_DIR}) # from tensorflow-lite
# add_subdirectory("${CMAKE_CURRENT_BINARY_DIR}/tensorflow-lite")
add_subdirectory(external/opencv) # from OpenCV

# Include the needed directories
include_directories(${TENSORFLOW_SOURCE_DIR})
include_directories(cmake-build-debug)
include_directories(cmake-build-debug/flatbuffers/include)
include_directories(external/opencv/include)
include_directories(build)

file(GLOB modules external/opencv/modules/*) # include every OpenCV module
FOREACH(module ${modules})
    include_directories(${module}/include)
ENDFOREACH()

# Add sources
add_executable(Roboter_Steuerung_jetson_nano src/main.cpp
        include/tensorflow/Tfml.hpp
        src/tensorflow/Tfml.cpp
        include/video/Vstream.hpp
	src/video/Vstream.cpp
        include/serial/Aserial.hpp
        src/serial/Aserial.cpp)

# link the libraries so that they build
target_link_libraries(Roboter_Steuerung_jetson_nano tensorflow-lite opencv_imgcodecs opencv_imgproc opencv_videoio opencv_highgui opencv_video)

# add the include for the projects cpp files
target_include_directories(Roboter_Steuerung_jetson_nano PUBLIC
        include
)






