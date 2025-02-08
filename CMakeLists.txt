cmake_minimum_required(VERSION 3.25)

include(ExternalProject)
project(wrapper)

# Windows configurations
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

# Update NDK path for Windows
set(ANDROID_NDK_PATH "$ENV{USERPROFILE}/android-ndk-r27c")
set(TOOLCHAIN "${ANDROID_NDK_PATH}/toolchains/llvm/prebuilt/windows-x86_64")

# Update compiler paths for Windows
set(CMAKE_C_COMPILER "${TOOLCHAIN}/bin/x86_64-linux-android35-clang.exe")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN}/bin/x86_64-linux-android35-clang++.exe")
set(C_COMPILER "${TOOLCHAIN}/bin/clang.exe")

set(CMAKE_C_FLAGS "-Wall -Werror -O3")
set(CMAKE_CXX_FLAGS "-Wall -Werror -O3")

set(CMDLINE_SOURCE cmdline.c)
set(HANDLE_SOURCE main.cpp)
set(MAIN_SOURCE main.c)
set(WRAPPER_SOURCE wrapper.c)

add_library(cmdline_object OBJECT ${CMDLINE_SOURCE})
add_library(handle_object OBJECT ${HANDLE_SOURCE})

add_executable(main ${MAIN_SOURCE} $<TARGET_OBJECTS:cmdline_object> $<TARGET_OBJECTS:handle_object>)

# Make library finding optional
find_library(ANDROIDAPPMUSIC_LIB androidappmusic PATHS ${CMAKE_SOURCE_DIR}/rootfs/system/lib64 NO_DEFAULT_PATH)
find_library(STORESERVICESCORE_LIB storeservicescore PATHS ${CMAKE_SOURCE_DIR}/rootfs/system/lib64 NO_DEFAULT_PATH)
find_library(MEDIAPLATFORM_LIB mediaplatform PATHS ${CMAKE_SOURCE_DIR}/rootfs/system/lib64 NO_DEFAULT_PATH)
find_library(CXX_SHARED_LIB c++_shared PATHS ${CMAKE_SOURCE_DIR}/rootfs/system/lib64 NO_DEFAULT_PATH)

# Create list of available libraries
set(AVAILABLE_LIBS)
if(ANDROIDAPPMUSIC_LIB)
    list(APPEND AVAILABLE_LIBS ${ANDROIDAPPMUSIC_LIB})
endif()
if(STORESERVICESCORE_LIB)
    list(APPEND AVAILABLE_LIBS ${STORESERVICESCORE_LIB})
endif()
if(MEDIAPLATFORM_LIB)
    list(APPEND AVAILABLE_LIBS ${MEDIAPLATFORM_LIB})
endif()
if(CXX_SHARED_LIB)
    list(APPEND AVAILABLE_LIBS ${CXX_SHARED_LIB})
endif()

# Link only available libraries
target_link_libraries(main
    ${AVAILABLE_LIBS}
)

if(WIN32)
    target_link_libraries(main wsock32 ws2_32)
endif()

set_target_properties(main PROPERTIES 
    COMPILE_DEFINITIONS "MyRelease"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/rootfs/system/bin"
)

link_directories(${CMAKE_SOURCE_DIR}/rootfs/system/lib64)

ExternalProject_Add(
    wrapper
    PREFIX ${CMAKE_BINARY_DIR}
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${C_COMPILER} -O3 -Wall -o wrapper ${WRAPPER_SOURCE}
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""
    DEPENDS main
)