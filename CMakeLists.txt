cmake_minimum_required(VERSION 3.6)
project(MPI_Wrapper)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

link_directories(../mpi-lib/lib)
include_directories(../mpi-lib/include)

set(SOURCE_FILES main.cpp msg_handlerABC.h MPI_Connect_Wrapper.cpp MPI_Connect_Wrapper.h MPI_Server.cpp MPI_Server.h MPI_util.h)
add_executable(MPI_Wrapper ${SOURCE_FILES})