cmake_minimum_required(VERSION 3.12)
set(PROPWARE_PATH C:/PropWare/share/PropWare)
find_package(PropWare REQUIRED)
project(MaD_Board C CXX)
set(BOARD propboe)
set(MODEL cmm)
set(COMMON_FLAGS)
INCLUDE_DIRECTORIES(include Librarys/NavKey Librarys/RA8876 Librarys/IOBoard Librarys/MCP23017 Librarys/SPI Librarys/DS3231 Librarys/ForceGauge )
create_simple_executable(${PROJECT_NAME}
    src/main.cpp
    src/MaD.cpp
    src/Keyboard.cpp
    src/Clock.cpp
    src/IOBoard.cpp
    src/StatusPage.cpp
    src/NavigationPage.cpp
    src/ManualPage.cpp
    Librarys/NavKey/i2cNavKey.cpp
    Librarys/RA8876/RA8876_Lite.cpp
    Librarys/MCP23017/MCP23017.cpp
    Librarys/SPI/SPI_Fast.c
    Librarys/DS3231/DS3231.cpp
    Librarys/ForceGauge/ForceGauge.cpp) 