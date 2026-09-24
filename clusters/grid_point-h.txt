/* grid_point.h -- класс GridPoint: точка, которая знает свой блок сетки.
   Поле делится на прямоугольные блоки, как карта города делится на
   квадраты: кроме собственных координат x и y точка хранит две целые
   координаты блока, в который она попала. По ним ближайшую точку можно
   искать не перебором всего облака, а только в соседних блоках.
   Автор: Горчак Дмитрий, 212 */

#ifndef GRID_POINT_H
#define GRID_POINT_H

#include <ostream>

#include "point.h"

/* Точка с двумя собственными координатами и двумя координатами блока */
class GridPoint : public Point {
public:
    GridPoint();
    GridPoint(float x, float y, int block_x, int block_y);

    int get_block_x() const;
    int get_block_y() const;
    void set_block_x(int block_x);              /* block_x >= 0 */
    void set_block_y(int block_y);              /* block_y >= 0 */

    void print(std::ostream &out) const override;
    const char *type_name() const override;

private:
    int block_x_;                               /* номер блока по оси x */
    int block_y_;                               /* номер блока по оси y */
};

#endif
