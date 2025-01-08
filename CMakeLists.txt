cmake_minimum_required(VERSION 3.10)
project(AdaptivePusher)

# 添加 头文件路径
include_directories(include/)

# 查找所有源文件
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 添加 main.cpp
add_executable(main ${SOURCES})

# 查找所有测试文件
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

# 添加 test.cpp
add_executable(test1 ${TEST_SOURCES})

# 链接 libcurl 库
find_package(CURL REQUIRED)
target_link_libraries(main CURL::libcurl)
target_link_libraries(test1 CURL::libcurl)

# 启用测试
enable_testing()

# 添加测试
add_test(NAME Test1 COMMAND test1)
