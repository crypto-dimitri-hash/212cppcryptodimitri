/* grid_point.cpp -- реализация класса GridPoint.
   Автор: Горчак Дмитрий, 212 */

#include "grid_point.h"

#include <iomanip>      /* std::setw, std::fixed, std::setprecision */
#include <stdexcept>    /* std::invalid_argument */

/* Автор: Горчак Дмитрий, 212
   Конструктор по умолчанию: точка в начале координат, блок (0, 0).
   Параметров нет.
   Ничего не возвращает. */
GridPoint::GridPoint() : Point(), block_x_(0), block_y_(0) {
}

/* Автор: Горчак Дмитрий, 212
   Конструктор точки с блоком сетки. Координаты передаются базовому
   классу Point, номера блока проверяются set-методами.
   Параметры:
      x, y             - собственные координаты точки
      block_x, block_y - целые координаты блока сетки
   Ничего не возвращает; при отрицательном номере блока бросает
   std::invalid_argument. */
GridPoint::GridPoint(float x, float y, int block_x, int block_y)
    : Point(x, y), block_x_(0), block_y_(0) {
    set_block_x(block_x);
    set_block_y(block_y);
}

/* Автор: Горчак Дмитрий, 212
   Читает номер блока по оси x.
   Параметров нет.
   Возвращает значение закрытого поля block_x_. */
int GridPoint::get_block_x() const {
    return block_x_;
}

/* Автор: Горчак Дмитрий, 212
   Читает номер блока по оси y.
   Параметров нет.
   Возвращает значение закрытого поля block_y_. */
int GridPoint::get_block_y() const {
    return block_y_;
}

/* Автор: Горчак Дмитрий, 212
   Записывает номер блока по оси x с проверкой значения.
   Параметры:
      block_x - номер блока (block_x >= 0)
   Ничего не возвращает; при отрицательном значении бросает исключение. */
void GridPoint::set_block_x(int block_x) {
    if (block_x < 0) {
        throw std::invalid_argument("block index must be >= 0");
    }
    block_x_ = block_x;
}

/* Автор: Горчак Дмитрий, 212
   Записывает номер блока по оси y с проверкой значения.
   Параметры:
      block_y - номер блока (block_y >= 0)
   Ничего не возвращает; при отрицательном значении бросает исключение. */
void GridPoint::set_block_y(int block_y) {
    if (block_y < 0) {
        throw std::invalid_argument("block index must be >= 0");
    }
    block_y_ = block_y;
}

/* Автор: Горчак Дмитрий, 212
   Печатает собственные координаты точки и координаты её блока.
   Параметры:
      out - поток вывода
   Ничего не возвращает. */
void GridPoint::print(std::ostream &out) const {
    out << std::fixed << std::setprecision(2);
    out << "GridPoint(x = " << std::setw(7) << get_x()
        << ", y = " << std::setw(7) << get_y()
        << ", block = [" << std::setw(2) << block_x_ << "," << std::setw(2)
        << block_y_ << "])";
}

/* Автор: Горчак Дмитрий, 212
   Название фигуры для таблицы.
   Параметров нет.
   Возвращает строку "GridPoint". */
const char *GridPoint::type_name() const {
    return "GridPoint";
}
