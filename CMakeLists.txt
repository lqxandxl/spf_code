cmake_minimum_required(VERSION 3.6)
project(spfcode)
set(CMAKE_CXX_STANDARD 9)
set(SOURCE_FILES main.cpp
        UtilService.h
        SerTreeNode.h SubInfoMng.h SerMessage.h
        SubInfoMng.cpp
        SerTreeNode.cpp UtilService.cpp
        JSON.cpp JSON.h JSONValue.cpp JSONValue.h ServiceTask.h ServiceTask.cpp PublishMng.h PublishMng.cpp SerMessage.cpp NTFMng.h NTFMng.cpp msgdef_rsc.h)
add_executable(spfcode ${SOURCE_FILES})
