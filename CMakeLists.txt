cmake_minimum_required(VERSION 3.12)
set(PROPWARE_PATH C:/PropWare/share/PropWare)
find_package(PropWare REQUIRED)
project(MaD_Board)
set(BOARD activityboard)
set(MODEL cmm)
INCLUDE_DIRECTORIES(include Librarys/I2CEncoder Librarys/NavKey Librarys/RA8876 Librarys/IOBoard)
create_simple_executable(${PROJECT_NAME}
    src/MaD.cpp
    Librarys/RA8876/RA8876_Lite.cpp
    Librarys/RA8876/DisplayTouchTest.cpp)