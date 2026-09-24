/* grid.h -- класс Grid: сетка блоков на плоскости.
   Поле делится на nx блоков по x и ny блоков по y. Сетка умеет
   сказать, в каком блоке лежит точка, и найти ближайшую точку облака,
   просматривая только соседние блоки вместо всего облака.
   Автор: Горчак Дмитрий, 212 */

#ifndef GRID_H
#define GRID_H

#include <string>
#include <vector>

#include "grid_point.h"

class Grid {
public:
    Grid(float x_min, float x_max, float y_min, float y_max, int nx, int ny);

    int get_nx() const;
    int get_ny() const;
    float cell_width() const;
    float cell_height() const;

    /* Номер блока по каждой оси для произвольной точки плоскости */
    int block_x_of(float x) const;
    int block_y_of(float y) const;

    /* Точка вместе с координатами своего блока */
    GridPoint make_point(float x, float y) const;

    /* Раскладывает точки по блокам: после этого поиск идёт по блокам */
    void fill(const std::vector<GridPoint> &points);

    /* Номер ближайшей точки к (x, y) полным перебором;
       checks -- сколько расстояний пришлось посчитать */
    int nearest_brute(const std::vector<GridPoint> &points, float x, float y,
                      long &checks) const;

    /* То же самое, но просматриваются только соседние блоки.
       Нужен предварительный вызов fill. */
    int nearest_by_blocks(const std::vector<GridPoint> &points, float x, float y,
                          long &checks) const;

    /* Линии сетки для gnuplot (набор отрезков) */
    bool save_lines(const std::string &filename) const;

private:
    /* Номера точек, попавших в каждый блок; блок (bx, by) лежит
       по индексу by * nx_ + bx */
    int cell_index(int block_x, int block_y) const;

    float x_min_;
    float x_max_;
    float y_min_;
    float y_max_;
    int nx_;
    int ny_;
    std::vector<std::vector<int> > cells_;
};

#endif
