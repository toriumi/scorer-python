# CMakeLists.txt

# cmake needs this line
cmake_minimum_required(VERSION 2.8)

# Define project name
project(LPRecogLib)

# Version Number
set(serial "2.0.0")

# Shared Library Version
set(soserial "1")

# Declare the executable target built from our sources
add_library(LPRecogLib SHARED LPRecogLib.cpp LPRecog.cpp CharRecognition.cpp Exception.cpp LicensePlateInfo.cpp PlateDetection.cpp PlateSegmentation.cpp common/LocalContrastNormalization.cpp common/RectFunctions.cpp HMAX/HMAX.cpp HMAX/S1.cpp HMAX/GaborS1.cpp HMAX/C1.cpp HMAX/commonFunctions.cpp shape/binary_func.cpp shape/shape_predictor.cpp)

set_target_properties(LPRecogLib PROPERTIES VERSION ${serial} SOVERSION ${soserial})

#----------------------------------------
# Find OpenCV, you may need to set OpenCV_DIR variable
# to the absolute path to the directory containing OpenCVConfig.cmake file
# via the command line or GUI
find_package(OpenCV 3 REQUIRED)

# If the package has been found, several variables will
# be set, you can find the full list with descriptions
# in the OpenCVConfig.cmake file.

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

target_include_directories(LPRecogLib PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(LPRecogLib ${OpenCV_LIBS})


#----------------------------------------
# Needs C++11 support
if(CMAKE_COMPILER_IS_GNUCXX)
  include(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
  if (COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else()
    message(STATUS "${CMAKE_CXX_COMPILER} has no C++11 support")
  endif()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL Clang)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
endif()


#----------------------------------------
# Find threading support
find_package(Threads)
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

find_package(OpenMP)
if (OPENMP_FOUND)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

link_libraries(${CMAKE_THREAD_LIBS_INIT})


# Install the shared library
install(TARGETS LPRecogLib DESTINATION lib)

# Install header file
install(FILES LPRecogLib.h LPRecog.h CharRecognition.h Exception.h LicensePlateInfo.h PlateDetection.h PlateSegmentation.h DESTINATION include)
install(FILES shape/shape_predictor.h shape/binary_func.h DESTINATION include/shape)
install(FILES HMAX/HMAX.h HMAX/S1.h HMAX/C1.h DESTINATION include/HMAX)

