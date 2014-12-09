# Install script for directory: /home/merz/BMN/GIT/trunk_220814

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Debug")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "0")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/base/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/fairtools/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/geane/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/generators/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/mpdgenerators/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/geobase/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/bmnfield/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/parbase/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/passive/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/trackbase/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/bmnbase/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/bmndata/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/tof/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/psd/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/sts/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/gem/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/globaltracking/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/recoil/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/dch/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/mwpc/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/tof1/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/QA/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/KF/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/cat/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/eventdisplay/cmake_install.cmake")
  INCLUDE("/home/merz/BMN/GIT/trunk_220814/build/macro/mpd_scheduler/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "/home/merz/BMN/GIT/trunk_220814/build/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "/home/merz/BMN/GIT/trunk_220814/build/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
