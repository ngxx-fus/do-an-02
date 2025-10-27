# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/codespace/esp-idf/components/bootloader/subproject"
  "/workspaces/do-an-02/build/bootloader"
  "/workspaces/do-an-02/build/bootloader-prefix"
  "/workspaces/do-an-02/build/bootloader-prefix/tmp"
  "/workspaces/do-an-02/build/bootloader-prefix/src/bootloader-stamp"
  "/workspaces/do-an-02/build/bootloader-prefix/src"
  "/workspaces/do-an-02/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/workspaces/do-an-02/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/workspaces/do-an-02/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
