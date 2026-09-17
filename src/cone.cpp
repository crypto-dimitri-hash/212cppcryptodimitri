/* cone.cpp -- реализация класса Cone.
   Автор: Горчак Дмитрий, 212 */

#include "cone.h"

#include <cmath>        /* std::sin */
#include <iomanip>      /* std::setw, std::fixed, std::setprecision */
#include <stdexcept>    /* std::invalid_argument */

static const double PI = 3.14159265358979323846;

/* Автор: Горчак Дмитрий, 212
   Конструктор по умолчанию: вырожденный конус (r = 0, h = 0).
   Параметров нет.
   Ничего не возвращает. */
Cone::Cone() : Circle(), h_(0.0f) {
}

/* Автор: Горчак Дмитрий, 212
   Конструктор конуса. Центр и радиус основания передаются базовому
   классу Circle, высота проверяется методом set_h.
   Параметры:
      x - абсцисса центра основания
      y - ордината центра основания
      r - радиус основания (r >= 0)
      h - высота (h >= 0)
   Ничего не возвращает; при r < 0 или h < 0 бросает std::invalid_argument. */
Cone::Cone(float x, float y, float r, float h) : Circle(x, y, r), h_(0.0f) {
    set_h(h);
}

/* Автор: Горчак Дмитрий, 212
   Читает высоту.
   Параметров нет.
   Возвращает значение закрытого поля h_. */
float Cone::get_h() const {
    return h_;
}

/* Автор: Горчак Дмитрий, 212
   Записывает высоту с проверкой значения.
   Параметры:
      h - новая высота (h >= 0)
   Ничего не возвращает; при h < 0 бросает std::invalid_argument. */
void Cone::set_h(float h) {
    if (h < 0.0f) {
        throw std::invalid_argument("height must be >= 0");
    }
    h_ = h;
}

/* Автор: Горчак Дмитрий, 212
   Вычислимый атрибут: объём V = S * h / 3, где S -- площадь основания,
   полученная методом area() базового класса Circle. Для круглого
   основания это объём конуса pi * r * r * h / 3.
   Параметров нет.
   Возвращает объём. */
double Cone::volume() const {
    return area() * static_cast<double>(h_) / 3.0;
}

/* Автор: Горчак Дмитрий, 212
   Объём пирамиды, в основании которой лежит правильный n-угольник,
   вписанный в круг радиуса r. Площадь такого n-угольника равна
   S(n) = n * r * r * sin(2 * pi / n) / 2; при росте n она стремится
   к площади круга, а объём -- к значению volume(). Метод оставлен,
   чтобы показать разницу между пирамидой и конусом численно.
   Параметры:
      n - число сторон основания (n >= 3)
   Возвращает объём; при n < 3 бросает std::invalid_argument. */
double Cone::volume_ngon(int n) const {
    double r = static_cast<double>(get_r());
    double base = 0.0;

    if (n < 3) {
        throw std::invalid_argument("polygon must have at least 3 sides");
    }
    base = n * r * r * std::sin(2.0 * PI / n) / 2.0;
    return base * static_cast<double>(h_) / 3.0;
}

/* Автор: Горчак Дмитрий, 212
   Высота боковой поверхности конуса над точкой плоскости (px, py).
   В центре основания она равна h, на границе круга -- нулю, вне
   основания конуса нет. Метод нужен для построения ландшафта:
   программа считает по нему сетку значений z(x, y) для gnuplot.
   Параметры:
      px - абсцисса точки плоскости
      py - ордината точки плоскости
   Возвращает высоту поверхности над точкой (0, если точка вне основания). */
double Cone::height_at(float px, float py) const {
    float distance = distance_to(Point(px, py));

    if (get_r() <= 0.0f || distance >= get_r()) {
        return 0.0;
    }
    /* Линейное убывание от вершины к краю основания -- образующая конуса */
    return static_cast<double>(h_) * (1.0 - distance / get_r());
}

/* Автор: Горчак Дмитрий, 212
   Печатает четыре числа, задающие конус, и его объём.
   Параметры:
      out - поток вывода
   Ничего не возвращает. */
void Cone::print(std::ostream &out) const {
    out << std::fixed << std::setprecision(2);
    out << "Cone(x = " << std::setw(7) << get_x()
        << ", y = " << std::setw(7) << get_y()
        << ", r = " << std::setw(6) << get_r()
        << ", h = " << std::setw(6) << h_
        << ", volume = " << std::setw(10) << volume() << ")";
}
