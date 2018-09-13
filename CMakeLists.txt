cmake_minimum_required(VERSION 3.10.2)
project(work_test)
set(CMAKE_CXX_STANDARD 14)
set(MY_FLAGS "-Wall -Wno-unused-variable -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MY_FLAGS}")

include_directories(include)

file(GLOB SOURCES "src/*.cpp")
file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

find_package(Boost 1.32 COMPONENTS program_options REQUIRED)

add_executable(apriori ${SOURCES})

target_link_libraries(apriori Boost::program_options)
