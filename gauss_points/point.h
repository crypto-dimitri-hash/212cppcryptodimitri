/* point.h -- класс Point: точка на плоскости (x, y).
   Первый уровень иерархии классов Point -> Circle -> Cone.
   Поля закрыты (private), доступ к ним -- только через get/set.
   Автор: Горчак Дмитрий, 212 */

#ifndef POINT_H
#define POINT_H

#include <ostream>

/* Точка на плоскости. Хранит два числа: x и y.
   Деструктор виртуальный, потому что от Point наследуют Circle и Cone
   и объекты могут использоваться через указатель на базовый класс. */
class Point {
public:
    Point();                        /* точка в начале координат (0, 0) */
    Point(float x, float y);
    virtual ~Point() = default;

    float get_x() const;            /* чтение закрытых полей */
    float get_y() const;
    void set_x(float x);            /* запись закрытых полей */
    void set_y(float y);

    /* Расстояние до другой точки: понадобится в задаче о кластерах */
    float distance_to(const Point &other) const;

    /* Печать объекта в поток (экран или файл).
       Метод виртуальный: у Circle и Cone своя строка вывода. */
    virtual void print(std::ostream &out) const;

    /* Название фигуры: "Point", "Circle" или "Cone".
       Нужно, чтобы одинаково печатать разнородные объекты. */
    virtual const char *type_name() const;

private:
    float x_;                       /* абсцисса */
    float y_;                       /* ордината */
};

#endif
