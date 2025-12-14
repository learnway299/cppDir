# interview/coding/CMakeLists.txt
# Interview Coding 题目模块

# 公共工具 (使用项目根目录的 util)
set(INTERVIEW_UTIL
    ${CMAKE_SOURCE_DIR}/util/Util.h
    ${CMAKE_SOURCE_DIR}/util/Util.cpp
)

# 测试入口
set(INTERVIEW_TEST_RUNNER
    ${CMAKE_CURRENT_SOURCE_DIR}/test_runner.h
    ${CMAKE_CURRENT_SOURCE_DIR}/test_runner.cpp
)

# 01-basic
file(GLOB INTERVIEW_01_BASIC
    "${CMAKE_CURRENT_SOURCE_DIR}/01-basic/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/01-basic/*.cpp"
)

# 02-linked-list
file(GLOB INTERVIEW_02_LINKED_LIST
    "${CMAKE_CURRENT_SOURCE_DIR}/02-linked-list/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/02-linked-list/*.cpp"
)

# 03-tree
file(GLOB INTERVIEW_03_TREE
    "${CMAKE_CURRENT_SOURCE_DIR}/03-tree/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/03-tree/*.cpp"
)

# 04-dp
file(GLOB INTERVIEW_04_DP
    "${CMAKE_CURRENT_SOURCE_DIR}/04-dp/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/04-dp/*.cpp"
)

# 05-string
file(GLOB INTERVIEW_05_STRING
    "${CMAKE_CURRENT_SOURCE_DIR}/05-string/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/05-string/*.cpp"
)

# 06-stack-queue
file(GLOB INTERVIEW_06_STACK_QUEUE
    "${CMAKE_CURRENT_SOURCE_DIR}/06-stack-queue/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/06-stack-queue/*.cpp"
)

# 07-hash-design
file(GLOB INTERVIEW_07_HASH_DESIGN
    "${CMAKE_CURRENT_SOURCE_DIR}/07-hash-design/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/07-hash-design/*.cpp"
)

# 08-sorting
file(GLOB INTERVIEW_08_SORTING
    "${CMAKE_CURRENT_SOURCE_DIR}/08-sorting/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/08-sorting/*.cpp"
)

# 09-graph
file(GLOB INTERVIEW_09_GRAPH
    "${CMAKE_CURRENT_SOURCE_DIR}/09-graph/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/09-graph/*.cpp"
)

# 10-smart-pointer
file(GLOB INTERVIEW_10_SMART_POINTER
    "${CMAKE_CURRENT_SOURCE_DIR}/10-smart-pointer/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/10-smart-pointer/*.cpp"
)

# 11-memory
file(GLOB INTERVIEW_11_MEMORY
    "${CMAKE_CURRENT_SOURCE_DIR}/11-memory/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/11-memory/*.cpp"
)

# 12-oop-design
file(GLOB INTERVIEW_12_OOP_DESIGN
    "${CMAKE_CURRENT_SOURCE_DIR}/12-oop-design/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/12-oop-design/*.cpp"
)

# 13-thread
file(GLOB INTERVIEW_13_THREAD
    "${CMAKE_CURRENT_SOURCE_DIR}/13-thread/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/13-thread/*.cpp"
)

# 14-lockfree
file(GLOB INTERVIEW_14_LOCKFREE
    "${CMAKE_CURRENT_SOURCE_DIR}/14-lockfree/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/14-lockfree/*.cpp"
)

# 15-template
file(GLOB INTERVIEW_15_TEMPLATE
    "${CMAKE_CURRENT_SOURCE_DIR}/15-template/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/15-template/*.cpp"
)

# 16-coroutine
file(GLOB INTERVIEW_16_COROUTINE
    "${CMAKE_CURRENT_SOURCE_DIR}/16-coroutine/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/16-coroutine/*.cpp"
)

# 17-backtracking
file(GLOB INTERVIEW_17_BACKTRACKING
    "${CMAKE_CURRENT_SOURCE_DIR}/17-backtracking/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/17-backtracking/*.cpp"
)

# 18-two-pointers
file(GLOB INTERVIEW_18_TWO_POINTERS
    "${CMAKE_CURRENT_SOURCE_DIR}/18-two-pointers/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/18-two-pointers/*.cpp"
)

# 19-bit-manipulation
file(GLOB INTERVIEW_19_BIT_MANIPULATION
    "${CMAKE_CURRENT_SOURCE_DIR}/19-bit-manipulation/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/19-bit-manipulation/*.cpp"
)

# 20-heap
file(GLOB INTERVIEW_20_HEAP
    "${CMAKE_CURRENT_SOURCE_DIR}/20-heap/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/20-heap/*.cpp"
)

# 21-system-design
file(GLOB INTERVIEW_21_SYSTEM_DESIGN
    "${CMAKE_CURRENT_SOURCE_DIR}/21-system-design/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/21-system-design/*.cpp"
)

# 汇总所有源文件
set(INTERVIEW_CODING_ALL_SOURCES
    ${INTERVIEW_UTIL}
    ${INTERVIEW_TEST_RUNNER}
    ${INTERVIEW_01_BASIC}
    ${INTERVIEW_02_LINKED_LIST}
    ${INTERVIEW_03_TREE}
    ${INTERVIEW_04_DP}
    ${INTERVIEW_05_STRING}
    ${INTERVIEW_06_STACK_QUEUE}
    ${INTERVIEW_07_HASH_DESIGN}
    ${INTERVIEW_08_SORTING}
    ${INTERVIEW_09_GRAPH}
    ${INTERVIEW_10_SMART_POINTER}
    ${INTERVIEW_11_MEMORY}
    ${INTERVIEW_12_OOP_DESIGN}
    ${INTERVIEW_13_THREAD}
    ${INTERVIEW_14_LOCKFREE}
    ${INTERVIEW_15_TEMPLATE}
    ${INTERVIEW_16_COROUTINE}
    ${INTERVIEW_17_BACKTRACKING}
    ${INTERVIEW_18_TWO_POINTERS}
    ${INTERVIEW_19_BIT_MANIPULATION}
    ${INTERVIEW_20_HEAP}
    ${INTERVIEW_21_SYSTEM_DESIGN}
    PARENT_SCOPE
)

# 设置 include 目录供外部使用
set(INTERVIEW_CODING_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
