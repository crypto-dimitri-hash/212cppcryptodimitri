/* grid.cpp -- реализация сетки блоков.
   Автор: Горчак Дмитрий, 212 */

#include "grid.h"

#include <cmath>        /* std::sqrt */
#include <fstream>      /* std::ofstream */
#include <iomanip>      /* std::fixed, std::setprecision */
#include <stdexcept>    /* std::invalid_argument */

/* Автор: Горчак Дмитрий, 212
   Конструктор сетки. Поле задаётся границами, число блоков -- nx и ny.
   Параметры:
      x_min, x_max, y_min, y_max - границы поля
      nx, ny                     - число блоков по каждой оси (>= 1)
   Ничего не возвращает; при неверных размерах бросает std::invalid_argument. */
Grid::Grid(float x_min, float x_max, float y_min, float y_max, int nx, int ny)
    : x_min_(x_min), x_max_(x_max), y_min_(y_min), y_max_(y_max), nx_(nx), ny_(ny) {
    if (nx < 1 || ny < 1) {
        throw std::invalid_argument("grid must have at least one block");
    }
    if (x_max <= x_min || y_max <= y_min) {
        throw std::invalid_argument("grid bounds are empty");
    }
    cells_.resize(static_cast<std::size_t>(nx_) * ny_);
}

/* Автор: Горчак Дмитрий, 212
   Число блоков по оси x.
   Параметров нет.
   Возвращает nx_. */
int Grid::get_nx() const {
    return nx_;
}

/* Автор: Горчак Дмитрий, 212
   Число блоков по оси y.
   Параметров нет.
   Возвращает ny_. */
int Grid::get_ny() const {
    return ny_;
}

/* Автор: Горчак Дмитрий, 212
   Ширина одного блока.
   Параметров нет.
   Возвращает размер блока по оси x. */
float Grid::cell_width() const {
    return (x_max_ - x_min_) / nx_;
}

/* Автор: Горчак Дмитрий, 212
   Высота одного блока.
   Параметров нет.
   Возвращает размер блока по оси y. */
float Grid::cell_height() const {
    return (y_max_ - y_min_) / ny_;
}

/* Автор: Горчак Дмитрий, 212
   Номер блока по оси x. Точки за границей поля относятся к крайнему блоку.
   Параметры:
      x - координата точки
   Возвращает номер блока от 0 до nx_ - 1. */
int Grid::block_x_of(float x) const {
    int index = static_cast<int>((x - x_min_) / cell_width());

    if (index < 0) {
        index = 0;
    }
    if (index > nx_ - 1) {
        index = nx_ - 1;
    }
    return index;
}

/* Автор: Горчак Дмитрий, 212
   Номер блока по оси y.
   Параметры:
      y - координата точки
   Возвращает номер блока от 0 до ny_ - 1. */
int Grid::block_y_of(float y) const {
    int index = static_cast<int>((y - y_min_) / cell_height());

    if (index < 0) {
        index = 0;
    }
    if (index > ny_ - 1) {
        index = ny_ - 1;
    }
    return index;
}

/* Автор: Горчак Дмитрий, 212
   Создаёт точку и сразу приписывает ей координаты её блока.
   Параметры:
      x, y - координаты точки
   Возвращает точку с четырьмя числами: x, y и два номера блока. */
GridPoint Grid::make_point(float x, float y) const {
    return GridPoint(x, y, block_x_of(x), block_y_of(y));
}

/* Автор: Горчак Дмитрий, 212
   Место блока в одномерном массиве блоков.
   Параметры:
      block_x, block_y - координаты блока
   Возвращает индекс блока. */
int Grid::cell_index(int block_x, int block_y) const {
    return block_y * nx_ + block_x;
}

/* Автор: Горчак Дмитрий, 212
   Раскладывает точки по блокам: в каждом блоке запоминаются номера
   попавших в него точек. Это и есть «оборот карты» -- список того,
   что лежит в каждом квадрате.
   Параметры:
      points - облако точек
   Ничего не возвращает. */
void Grid::fill(const std::vector<GridPoint> &points) {
    for (std::size_t i = 0; i < cells_.size(); ++i) {
        cells_[i].clear();
    }
    for (std::size_t i = 0; i < points.size(); ++i) {
        int index = cell_index(points[i].get_block_x(), points[i].get_block_y());

        cells_[index].push_back(static_cast<int>(i));
    }
}

/* Автор: Горчак Дмитрий, 212
   Ближайшая точка облака полным перебором: считаются расстояния до
   всех точек. Это простой и надёжный способ, но он требует n проверок.
   Параметры:
      points - облако точек
      x, y   - координаты новой точки
      checks - сюда записывается число посчитанных расстояний
   Возвращает номер ближайшей точки или -1, если облако пустое. */
int Grid::nearest_brute(const std::vector<GridPoint> &points, float x, float y,
                        long &checks) const {
    int best = -1;
    float best_distance = 0.0f;

    checks = 0;
    for (std::size_t i = 0; i < points.size(); ++i) {
        float distance = points[i].distance_to(Point(x, y));

        ++checks;
        if (best == -1 || distance < best_distance) {
            best = static_cast<int>(i);
            best_distance = distance;
        }
    }
    return best;
}

/* Автор: Горчак Дмитрий, 212
   Ближайшая точка облака с помощью сетки. Сначала просматривается блок
   самой точки, затем кольцо соседних блоков, затем следующее кольцо и
   так далее. Поиск прекращается, когда найденное расстояние меньше, чем
   расстояние до границы просмотренной области: дальше ближе не будет.
   Параметры:
      points - облако точек (должно быть разложено вызовом fill)
      x, y   - координаты новой точки
      checks - сюда записывается число посчитанных расстояний
   Возвращает номер ближайшей точки или -1, если облако пустое. */
int Grid::nearest_by_blocks(const std::vector<GridPoint> &points, float x, float y,
                            long &checks) const {
    int center_x = block_x_of(x);
    int center_y = block_y_of(y);
    int best = -1;
    float best_distance = 0.0f;
    int max_ring = (nx_ > ny_) ? nx_ : ny_;
    float cell = (cell_width() < cell_height()) ? cell_width() : cell_height();

    checks = 0;
    for (int ring = 0; ring <= max_ring; ++ring) {
        for (int by = center_y - ring; by <= center_y + ring; ++by) {
            for (int bx = center_x - ring; bx <= center_x + ring; ++bx) {
                /* внутри кольца просматриваем только его границу */
                bool on_ring = (bx == center_x - ring) || (bx == center_x + ring) ||
                               (by == center_y - ring) || (by == center_y + ring);

                if (!on_ring || bx < 0 || by < 0 || bx >= nx_ || by >= ny_) {
                    continue;
                }

                const std::vector<int> &cell_points = cells_[cell_index(bx, by)];
                for (std::size_t k = 0; k < cell_points.size(); ++k) {
                    int number = cell_points[k];
                    float distance = points[number].distance_to(Point(x, y));

                    ++checks;
                    if (best == -1 || distance < best_distance) {
                        best = number;
                        best_distance = distance;
                    }
                }
            }
        }
        /* Если ближайшая точка ближе, чем граница просмотренных блоков,
           дальше искать незачем. */
        if (best != -1 && best_distance <= ring * cell) {
            break;
        }
    }
    return best;
}

/* Автор: Горчак Дмитрий, 212
   Сохраняет линии сетки для gnuplot: каждая линия задаётся двумя
   точками, между парами оставлена пустая строка.
   Параметры:
      filename - имя файла с линиями
   Возвращает true при успешной записи. */
bool Grid::save_lines(const std::string &filename) const {
    std::ofstream file(filename.c_str());

    if (!file.is_open()) {
        return false;
    }

    file << "# x y  (grid lines, two points per line)\n";
    file << std::fixed << std::setprecision(4);
    for (int i = 0; i <= nx_; ++i) {
        float x = x_min_ + i * cell_width();

        file << x << ' ' << y_min_ << '\n' << x << ' ' << y_max_ << "\n\n";
    }
    for (int j = 0; j <= ny_; ++j) {
        float y = y_min_ + j * cell_height();

        file << x_min_ << ' ' << y << '\n' << x_max_ << ' ' << y << "\n\n";
    }

    file.close();
    return true;
}
