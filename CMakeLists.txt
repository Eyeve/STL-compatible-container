cmake_minimum_required(VERSION 3.10)
project(ct-c24-lw-containers-Eyeve)

set(CMAKE_CXX_STANDARD 23)

# Добавляем исходные файлы проекта
add_executable(${PROJECT_NAME} tests.cpp)

# Подключаем GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/release-1.12.1.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
)
# Указываем, что не нужно использовать Google Mock
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Подключаем и собираем GoogleTest
FetchContent_MakeAvailable(googletest)

# Линковка с GoogleTest
target_link_libraries(${PROJECT_NAME} gtest gtest_main)
