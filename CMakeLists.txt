cmake_minimum_required(VERSION 3.12)
project(Triangalizer)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
find_package( Threads )

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp)

add_executable(Triangalizer main.cpp Triangle.cpp Triangle.h PointLine.cpp PointLine.h SlopeLine.cpp SlopeLine.h addons.cpp addons.h Vertex.cpp Vertex.h)

target_link_libraries(Triangalizer sfml-graphics sfml-audio)
target_link_libraries(Triangalizer ${CMAKE_THREAD_LIBS_INIT} )