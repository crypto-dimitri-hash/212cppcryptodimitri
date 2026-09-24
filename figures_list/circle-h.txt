/* circle.h -- класс Circle: круг на плоскости (x, y, r).
   Второй уровень иерархии: Circle -- это Point плюс радиус.
   Площадь -- вычислимый атрибут: не хранится в поле, а считается методом.
   Автор: Горчак Дмитрий, 212 */

#ifndef CIRCLE_H
#define CIRCLE_H

#include <ostream>

#include "point.h"

/* Круг: центр берётся у Point, добавляется только радиус.
   Отрицательный радиус -- ошибка: set_r бросает std::invalid_argument. */
class Circle : public Point {
public:
    Circle();                                   /* круг нулевого радиуса в начале координат */
    Circle(float x, float y, float r);

    float get_r() const;
    void set_r(float r);                        /* r >= 0, иначе std::invalid_argument */

    /* Вычислимый атрибут: площадь круга S = pi * r * r.
       Считается в double, чтобы при умножении не терять точность float. */
    double area() const;

    void print(std::ostream &out) const override;
    const char *type_name() const override;

private:
    float r_;                                   /* радиус */
};

#endif
