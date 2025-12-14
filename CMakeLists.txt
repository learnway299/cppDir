# Created by learyuan on 25-6-8.
cmake_minimum_required(VERSION 3.31)
project(Cpp_Interview)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 原有源文件
set(ORIGINAL_SOURCES
        sort/quickSort/QuickSort.cpp
        sort/quickSort/QuickSort.h
        sort/mergeSort/MergeSort.cpp
        sort/mergeSort/MergeSort.h
        sort/other/StrCountSort.cpp
        sort/other/StrCountSort.h
        stack/IncrementalSequence.cpp
        stack/IncrementalSequence.h
        stack/Subsequence.cpp
        stack/Subsequence.h
        stack/SubSet.cpp
        stack/SubSet.h
        stack/TreeNodeTraversal.cpp
        stack/TreeNodeTraversal.h
        thread/MessageQueue.cpp
        thread/MessageQueue.h
        thread/AlternatePrint.cpp
        thread/AlternatePrint.h
)

# 引入 interview/coding 子模块
add_subdirectory(interview/coding)

# 添加线程库支持 (required for thread, mutex, etc.)
find_package(Threads REQUIRED)

# 主可执行文件
add_executable(Cpp_Interview main.cpp
        ${ORIGINAL_SOURCES}
        ${INTERVIEW_CODING_ALL_SOURCES}
)

# 添加 include 目录
target_include_directories(Cpp_Interview PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${INTERVIEW_CODING_INCLUDE_DIR}
)

# 链接线程库
target_link_libraries(Cpp_Interview Threads::Threads)
