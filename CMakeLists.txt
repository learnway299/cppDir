#Created by learyuan on 25-6-8.
cmake_minimum_required(VERSION 3.31)
project(project1)

set(CMAKE_CXX_STANDARD 20)

add_executable(project1 main.cpp
        sort/util/Util.cpp
        sort/util/Util.h
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
        thread/AlternatePrint.h)
