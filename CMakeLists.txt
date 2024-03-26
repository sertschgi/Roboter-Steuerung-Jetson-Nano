cmake_minimum_required(VERSION 3.10)

include(ExternalProject)

project(aijnano)

set(CMAKE_CXX_STANDARD 17)
set(TFLITE_ENABLE_GPU true)

set(TENSORFLOW_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/tensorflow")
set(TFLITE_SOURCE_DIR "${TENSORFLOW_SOURCE_DIR}/tensorflow/lite")

set(POCL_BUILD OFF CACHE BOOL "Build with pocl or not.")
set(POCL_CLANG_VERSION 7 CACHE STRING "The llvm/clang version your using. ( <= 7 requiered)")

# Add CMakeLists.txt
add_subdirectory(${TFLITE_SOURCE_DIR}) # from tensorflow-lite
# add_subdirectory("${CMAKE_CURRENT_BINARY_DIR}/tensorflow-lite")
add_subdirectory(external/opencv) # from OpenCV
add_subdirectory(external/ClParser)

if (${POCL_BUILD})
	ExternalProject_Add(
		pocl
		PREFIX external/pocl
		GIT_REPOSITORY https://github.com/pocl/pocl.git
		GIT_TAG release_1_2
		CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=/usr/local/pocl -DENABLE_CUDA=ON -DWITH_LLVM_CONFIG=/usr/bin/llvm-config-${POCL_CLANG_VERSION} -DLLVM_OPT=/usr/bin/opt-${POCL_CLANG_VERSION} -DLLVM_LLC=/usr/bin/llc-${POCL_CLANG_VERSION} -DLLVM_AS=/usr/bin/llvm-as-${POCL_CLANG_VERSION} -DLLVM_LINK=/usr/bin/llvm-link-${POCL_CLANG_VERSION} -DLLVM_LLI=/usr/bin/lli-${POCL_CLANG_VERSION} -DCLANG=/usr/bin/clang-${POCL_CLANG_VERSION} -DCLANGXX=/usr/bin/clang++-${POCL_CLANG_VERSION})

	set(POCL_INSTALL_SCRIPT_PATH "${CMAKE_SOURCE_DIR}/install_scripts/install_pocl.sh")

	add_custom_command(
		TARGET pocl
		POST_BUILD
		COMMAND chmod +x ${POCL_INSTALL_SCRIPT_PATH}
		COMMAND bash -i -c ${POCL_INSTALL_SCRIPT_PATH}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

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
add_executable(aijnano src/main.cpp
        include/tensorflow/Tfml.hpp
        src/tensorflow/Tfml.cpp
        include/video/Vstream.hpp
	src/video/Vstream.cpp
        include/serial/Aserial.hpp
        src/serial/Aserial.cpp)

# link the libraries so that they build
target_link_libraries(aijnano ClParser tensorflow-lite opencv_imgcodecs opencv_imgproc opencv_videoio opencv_highgui opencv_video)

# add the include for the projects cpp files
target_include_directories(aijnano PUBLIC
        include
)






