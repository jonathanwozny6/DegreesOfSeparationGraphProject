### Copy and paste this into the CMakeLists.txt

cmake_minimum_required(VERSION 3.17)
project(untitled1)

include_directories(/usr/local/opt/curl/include/)

set(CMAKE_CXX_STANDARD 14)

set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED)

find_package(wxWidgets COMPONENTS core base REQUIRED)

find_library(FoundationLib CoreFoundation)
message("LIB: ${FoundationLib}")

include("${wxWidgets_USE_FILE}")


add_executable(untitled1 main.cpp cApp.cpp cApp.h cFrame.cpp cFrame.h)

include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(untitled1 ${CURL_LIBRARIES})

target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
