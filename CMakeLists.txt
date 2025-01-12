cmake_minimum_required(VERSION 3.10)
project(AdaptivePusher)

include(FetchContent)

FetchContent_Declare(
    lz4
    GIT_REPOSITORY https://github.com/lz4/lz4.git
    GIT_TAG        v1.10.0 # 指定版本号
)
FetchContent_MakeAvailable(lz4)

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
target_link_libraries(main CURL::libcurl lz4)
target_link_libraries(test1 CURL::libcurl lz4)

# 启用测试
enable_testing()

# 添加测试
add_test(NAME Test1 COMMAND test1)
