cmake_minimum_required(VERSION 3.6)
project(spfcode)
set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES main.cpp
        UtilService.h
        TreeNode.h SubInfoMng.h Message.h
        SubInfoMng.cpp
        TreeNode.cpp UtilService.cpp
        JSON.cpp JSON.h JSONValue.cpp JSONValue.h Proxy.h Proxy.cpp PublishMng.h PublishMng.cpp Message.cpp NTFMng.h NTFMng.cpp)
add_executable(spfcode ${SOURCE_FILES})
