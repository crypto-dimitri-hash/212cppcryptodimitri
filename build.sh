#!/bin/bash
# build.sh -- сборка программ через g++ (MSYS2 / Linux).
# Автор: Горчак Дмитрий, 212
#
# Запуск из этой папки:
#   bash build.sh
# Готовые программы складываются в папку bin, результаты работы -- в папку out.

FLAGS="-std=c++17 -Wall -Wextra"
CLASSES="point.cpp circle.cpp cone.cpp figures_io.cpp"
BIN=bin
OK=0
FAIL=0

mkdir -p $BIN

# build ИМЯ ФАЙЛ_ПРОГРАММЫ -- компилирует программу вместе с классами
# и по коду завершения g++ ($? равен 0 при успехе) сообщает результат
build() {
    local name=$1
    local app=$2

    g++ $FLAGS $app $CLASSES -o $BIN/$name
    if [ $? -eq 0 ]; then
        echo "  OK    $name"
        OK=$((OK + 1))
    else
        echo "  FAIL  $name"
        FAIL=$((FAIL + 1))
    fi
}

echo "classes Point -> Circle -> Cone"
build figures main.cpp            # таблица фигур и данные для gnuplot
build figures_list list_demo.cpp  # двунаправленный список STL

echo "done: $OK OK, $FAIL failed"
echo "run:  ./bin/figures  и затем  gnuplot plot_figures.gp"
