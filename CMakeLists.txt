# CMakeLists.txt -- сборка задачи «Конусы на плоскости» (занятие 6: CMake).
# Автор: Горчак Дмитрий, 212
#
# Сборка из этой папки:
#   cmake -B build
#   cmake --build build
# Классы собираются в библиотеку cone_lib, а обе программы её подключают.

cmake_minimum_required(VERSION 3.10)

project(Cones LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Библиотека классов: Point -> Circle -> Cone и ввод-вывод
add_library(cone_lib
    src/point.cpp
    src/circle.cpp
    src/cone.cpp
    src/cone_io.cpp
)
target_include_directories(cone_lib PUBLIC include)

# Этап к 19.09: объекты из файла, вычислимые атрибуты, данные для gnuplot
add_executable(cones_app app/main.cpp)
target_link_libraries(cones_app PRIVATE cone_lib)

# Этап к 26.09: двунаправленный список STL и упорядочивание
add_executable(cones_list app/list_demo.cpp)
target_link_libraries(cones_list PRIVATE cone_lib)

if(MSVC)
    target_compile_options(cone_lib PRIVATE /W4)
else()
    target_compile_options(cone_lib PRIVATE -Wall -Wextra)
endif()
