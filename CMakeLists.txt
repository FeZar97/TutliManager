cmake_minimum_required(VERSION 3.14)

project(TutliManager LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET("OpenCV_DIR" "${CMAKE_CURRENT_SOURCE_DIR}\\..\\opencv\\build_opencv")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets WinExtras REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets WinExtras REQUIRED)
find_package(OpenCV REQUIRED)

MESSAGE(STATUS "Include dirs ${OpenCV_INCLUDE_DIRS}")
MESSAGE(STATUS "LINK LIBRARIES ${OpenCV_LIBS}")
MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

include_directories(${OpenCV_INCLUDE_DIRS})

file(GLOB_RECURSE HDRS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(TutliManager
  src/main.cpp
  ${HDRS} ${SRCS}
)
target_link_libraries(TutliManager
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::WinExtras
    ${OpenCV_LIBS}
)
