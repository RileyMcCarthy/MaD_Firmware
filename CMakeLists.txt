cmake_minimum_required(VERSION 3.12)
set(PROPWARE_PATH C:/PropWare/share/PropWare)
find_package(PropWare REQUIRED)
project(MaD_Board)
INCLUDE_DIRECTORIES(include Librarys/I2CEncoder Librarys/RA8876)
create_simple_executable(${PROJECT_NAME}
    src/MaD.cpp
    Librarys/I2CEncoder/i2cEncoderLibV2.cpp
    Librarys/I2CEncoder/EncoderTest.cpp
    Librarys/NavKey/i2CNavKey.cpp
    Librarys/RA8876/RA8876_Lite.cpp
    Librarys/RA8876/DisplayTouchTest.cpp)