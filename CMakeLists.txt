cmake_minimum_required(VERSION 3.16)
project(ChassisDcoder LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(chassisdcoder WIN32
    src/main.cpp
    src/http.cpp
    src/vin.cpp
    src/chassis.cpp
    src/chassis_bmw.cpp
    src/chassis_mini.cpp
    src/chassis_mercedes.cpp
    src/chassis_mazda.cpp
    src/chassis_toyota.cpp
    src/chassis_nissan.cpp
    src/chassis_subaru.cpp
)

target_include_directories(chassisdcoder PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# MinGW/Windows: link WinHTTP for HTTPS requests to NHTSA.
if (WIN32)
    target_link_libraries(chassisdcoder PRIVATE winhttp)
endif()

if (MSVC)
    target_compile_options(chassisdcoder PRIVATE /W4)
else()
    target_compile_options(chassisdcoder PRIVATE -Wall -Wextra -Wpedantic)
    if (MINGW)
        target_link_options(chassisdcoder PRIVATE -static -static-libgcc -static-libstdc++)
    endif()

endif()

