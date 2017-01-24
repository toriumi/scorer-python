# Install script for directory: /home/pi/LicensePlateRecognizer/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libLPRecogLib.so.2.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libLPRecogLib.so.1"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libLPRecogLib.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/pi/LicensePlateRecognizer/src/libLPRecogLib.so.2.0.0"
    "/home/pi/LicensePlateRecognizer/src/libLPRecogLib.so.1"
    "/home/pi/LicensePlateRecognizer/src/libLPRecogLib.so"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libLPRecogLib.so.2.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libLPRecogLib.so.1"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libLPRecogLib.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_REMOVE
           FILE "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/home/pi/LicensePlateRecognizer/src/LPRecogLib.h"
    "/home/pi/LicensePlateRecognizer/src/LPRecog.h"
    "/home/pi/LicensePlateRecognizer/src/CharRecognition.h"
    "/home/pi/LicensePlateRecognizer/src/Exception.h"
    "/home/pi/LicensePlateRecognizer/src/LicensePlateInfo.h"
    "/home/pi/LicensePlateRecognizer/src/PlateDetection.h"
    "/home/pi/LicensePlateRecognizer/src/PlateSegmentation.h"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/shape" TYPE FILE FILES
    "/home/pi/LicensePlateRecognizer/src/shape/shape_predictor.h"
    "/home/pi/LicensePlateRecognizer/src/shape/binary_func.h"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/HMAX" TYPE FILE FILES
    "/home/pi/LicensePlateRecognizer/src/HMAX/HMAX.h"
    "/home/pi/LicensePlateRecognizer/src/HMAX/S1.h"
    "/home/pi/LicensePlateRecognizer/src/HMAX/C1.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

file(WRITE "/home/pi/LicensePlateRecognizer/src/${CMAKE_INSTALL_MANIFEST}" "")
foreach(file ${CMAKE_INSTALL_MANIFEST_FILES})
  file(APPEND "/home/pi/LicensePlateRecognizer/src/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
endforeach()
