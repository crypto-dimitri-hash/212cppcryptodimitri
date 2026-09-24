#!/bin/bash

FLAGS="-std=c++17 -Wall -Wextra"
CLASSES="point.cpp circle.cpp cone.cpp figures_io.cpp grid_point.cpp grid.cpp gauss.cpp scatter.cpp cluster.cpp"
BIN=bin
OK=0
FAIL=0

mkdir -p $BIN

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
build figures main.cpp
build figures_list list_demo.cpp
build gauss_points gauss_points.cpp
build gauss_demo gauss_demo.cpp
build clusters clusters_demo.cpp

echo "done: $OK OK, $FAIL failed"
