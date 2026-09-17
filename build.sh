#!/bin/bash
# build.sh -- сборка программ задачи «Конусы на плоскости» через g++, без CMake.
# Автор: Горчак Дмитрий, 212
#
# Запуск из папки cones:
#   bash build.sh
# Готовые программы складываются в папку bin (в Windows g++ сам добавит .exe).

FLAGS="-std=c++17 -Wall -Wextra"
SRC="src/point.cpp src/circle.cpp src/cone.cpp src/cone_io.cpp"
BIN=bin
OK=0
FAIL=0

mkdir -p $BIN

# build ИМЯ ФАЙЛ_ПРОГРАММЫ -- компилирует программу вместе с классами
# и по коду завершения g++ ($? равен 0 при успехе) сообщает результат
build() {
    local name=$1
    local app=$2

    g++ $FLAGS -Iinclude $app $SRC -o $BIN/$name
    if [ $? -eq 0 ]; then
        echo "  OK    $name"
        OK=$((OK + 1))
    else
        echo "  FAIL  $name"
        FAIL=$((FAIL + 1))
    fi
}

echo "cones: classes Point -> Circle -> Cone"
build cones_app app/main.cpp        # этап к 19.09: объекты и gnuplot
build cones_list app/list_demo.cpp  # этап к 26.09: двунаправленный список STL

echo "done: $OK OK, $FAIL failed"
echo "run:  ./bin/cones_app  и затем  gnuplot plot/plot_cones.gp"
