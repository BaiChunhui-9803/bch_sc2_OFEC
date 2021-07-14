cmake_minimum_required(VERSION 3.10)
project(OFEC_sc2)
find_package(Threads)
set(CMAKE_CXX_FLAGS -pthread)
set(OFEC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
IF (WIN32)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHs")
ENDIF (WIN32)
add_definitions(-DOFEC_DIR="${OFEC_DIR}")
# include(Files.cmake)
include(SrcFiles.cmake)
target_compile_features(OFEC_sc2 PRIVATE cxx_std_17)

# C/C++ 常规 附加包含目录
include_directories(../../include)
include_directories(../../contrib/protobuf/src)
include_directories(../../contrib/civetweb/include)
include_directories(../../build/contrib/ipv6-parse)
include_directories(../../build/generated)
include_directories(../../contrib/SDL-mirror/include)

# 链接器 输入 附加依赖项
target_link_libraries(${PROJECT_NAME} 
../../bin/sc2apid
../../bin/sc2libd
../../bin/sc2utilsd
../../bin/sc2rendererd
../../bin/sc2protocold
../../bin/libprotobufd
../../bin/civetweb
../../contrib/ipv6-parse/bin/ipv6-parse
../../bin/SDL2
winmm.lib
imm32.lib
version.lib
dinput8.lib
)