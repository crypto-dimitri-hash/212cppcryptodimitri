/* figures_io.cpp -- реализация ввода-вывода для фигур.
   Автор: Горчак Дмитрий, 212 */

#include "figures_io.h"

#include <cmath>        /* std::cos, std::sin */
#include <filesystem>   /* создание папки для результатов */
#include <fstream>      /* std::ifstream, std::ofstream */
#include <iomanip>      /* std::setw, std::fixed, std::setprecision */
#include <sstream>      /* std::istringstream -- разбор одной строки файла */
#include <stdexcept>    /* std::invalid_argument */

/* Автор: Горчак Дмитрий, 212
   Создаёт фигуру по числам, прочитанным из строки файла. Количество
   чисел определяет фигуру: 2 -- точка, 3 -- круг, 4 -- конус.
   Параметры:
      value - массив прочитанных чисел
      count - сколько чисел прочитано (2, 3 или 4)
   Возвращает указатель на созданную фигуру; при count вне 2..4 --
   пустой указатель. Бросает std::invalid_argument при r < 0 или h < 0. */
static FigurePtr make_figure(const float value[4], int count) {
    if (count == 2) {
        return FigurePtr(new Point(value[0], value[1]));
    }
    if (count == 3) {
        return FigurePtr(new Circle(value[0], value[1], value[2]));
    }
    if (count == 4) {
        return FigurePtr(new Cone(value[0], value[1], value[2], value[3]));
    }
    return FigurePtr();
}

/* Автор: Горчак Дмитрий, 212
   Читает фигуры из текстового файла. В строке от двух до четырёх чисел.
   Пустые строки и строки, начинающиеся с '#', пропускаются. Строка с
   неверными данными не прерывает чтение: о ней сообщается в log.
   Параметры:
      filename - имя файла с исходными данными
      figures  - вектор, в который добавляются прочитанные фигуры
      log      - поток для сообщений об ошибочных строках
   Возвращает true, если файл открылся, и false, если его нет. */
bool load_figures(const std::string &filename, std::vector<FigurePtr> &figures,
                  std::ostream &log) {
    std::ifstream file(filename.c_str());
    std::string line;
    int line_number = 0;

    if (!file.is_open()) {
        return false;
    }

    while (std::getline(file, line)) {
        float value[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        int count = 0;

        ++line_number;
        if (line.empty() || line[0] == '#') {
            continue;                       /* комментарий или пустая строка */
        }

        /* Читаем числа по одному, пока они есть, но не больше четырёх */
        std::istringstream input(line);
        while (count < 4 && (input >> value[count])) {
            ++count;
        }

        if (count < 2) {
            log << "  line " << line_number << " skipped: need 2 to 4 numbers\n";
            continue;
        }

        /* Проверка значений спрятана в set-методах классов, поэтому
           здесь достаточно перехватить исключение. */
        try {
            figures.push_back(make_figure(value, count));
        } catch (const std::invalid_argument &error) {
            log << "  line " << line_number << " skipped: " << error.what() << '\n';
        }
    }

    file.close();
    return true;
}

/* Автор: Горчак Дмитрий, 212
   Площадь основания фигуры. Проверка типа сделана через dynamic_cast:
   он возвращает нулевой указатель, если фигура не круг и не конус.
   Параметры:
      figure - указатель на фигуру
   Возвращает площадь круга или 0 для точки. */
double area_of(const FigurePtr &figure) {
    const Circle *circle = dynamic_cast<const Circle *>(figure.get());

    if (circle == nullptr) {
        return 0.0;                 /* у точки площади нет */
    }
    return circle->area();
}

/* Автор: Горчак Дмитрий, 212
   Объём фигуры. Объём есть только у конуса, поэтому он и считается
   отдельно, по запросу, а не хранится у каждой фигуры.
   Параметры:
      figure - указатель на фигуру
   Возвращает объём конуса или 0 для точки и круга. */
double volume_of(const FigurePtr &figure) {
    const Cone *cone = dynamic_cast<const Cone *>(figure.get());

    if (cone == nullptr) {
        return 0.0;
    }
    return cone->volume();
}

/* Автор: Горчак Дмитрий, 212
   Определяет границы поля по x и y. Для круга и конуса учитывается
   радиус основания, точка занимает одну позицию. К найденному
   прямоугольнику добавляется запас, чтобы фигуры не упирались в рамку.
   Параметры:
      figures        - вектор фигур
      margin_percent - запас в процентах от размера поля
   Возвращает структуру с границами; для пустого вектора -- нули. */
FieldBounds field_bounds(const std::vector<FigurePtr> &figures, float margin_percent) {
    FieldBounds bounds = {0.0f, 0.0f, 0.0f, 0.0f};
    float margin = 0.0f;
    bool first = true;

    for (std::size_t i = 0; i < figures.size(); ++i) {
        const Circle *circle = dynamic_cast<const Circle *>(figures[i].get());
        float radius = (circle == nullptr) ? 0.0f : circle->get_r();
        float x = figures[i]->get_x();
        float y = figures[i]->get_y();

        if (first) {
            bounds.x_min = x - radius;
            bounds.x_max = x + radius;
            bounds.y_min = y - radius;
            bounds.y_max = y + radius;
            first = false;
            continue;
        }
        if (x - radius < bounds.x_min) {
            bounds.x_min = x - radius;
        }
        if (x + radius > bounds.x_max) {
            bounds.x_max = x + radius;
        }
        if (y - radius < bounds.y_min) {
            bounds.y_min = y - radius;
        }
        if (y + radius > bounds.y_max) {
            bounds.y_max = y + radius;
        }
    }

    /* Запас считаем от большей стороны, чтобы поле не искажалось */
    margin = (bounds.x_max - bounds.x_min > bounds.y_max - bounds.y_min)
                 ? (bounds.x_max - bounds.x_min)
                 : (bounds.y_max - bounds.y_min);
    margin = margin * margin_percent / 100.0f;

    bounds.x_min -= margin;
    bounds.x_max += margin;
    bounds.y_min -= margin;
    bounds.y_max += margin;
    return bounds;
}

/* Автор: Горчак Дмитрий, 212
   Печатает таблицу фигур: номер, тип, хранимые числа и вычислимые
   атрибуты. У точки нет ни радиуса, ни высоты, ни площади -- в этих
   столбцах ставится прочерк.
   Параметры:
      figures - вектор фигур
      out     - поток вывода (экран или файл)
   Ничего не возвращает. */
void print_table(const std::vector<FigurePtr> &figures, std::ostream &out) {
    double total_area = 0.0;
    double total_volume = 0.0;

    out << std::setw(4) << "N" << std::setw(9) << "type" << std::setw(9) << "x"
        << std::setw(9) << "y" << std::setw(9) << "r" << std::setw(9) << "h"
        << std::setw(12) << "area" << std::setw(12) << "volume" << '\n';
    out << std::string(73, '-') << '\n';

    for (std::size_t i = 0; i < figures.size(); ++i) {
        const FigurePtr &figure = figures[i];
        const Circle *circle = dynamic_cast<const Circle *>(figure.get());
        const Cone *cone = dynamic_cast<const Cone *>(figure.get());

        out << std::setw(4) << i + 1 << std::setw(9) << figure->type_name();
        out << std::fixed << std::setprecision(2);
        out << std::setw(9) << figure->get_x() << std::setw(9) << figure->get_y();

        if (circle == nullptr) {
            out << std::setw(9) << "-" << std::setw(9) << "-" << std::setw(12) << "-";
        } else {
            out << std::setw(9) << circle->get_r();
            if (cone == nullptr) {
                out << std::setw(9) << "-";
            } else {
                out << std::setw(9) << cone->get_h();
            }
            out << std::setw(12) << circle->area();
        }

        if (cone == nullptr) {
            out << std::setw(12) << "-" << '\n';
        } else {
            out << std::setw(12) << cone->volume() << '\n';
        }

        total_area += area_of(figure);
        total_volume += volume_of(figure);
    }

    out << std::string(73, '-') << '\n';
    out << std::setw(4) << figures.size() << std::setw(45) << "total:"
        << std::setw(12) << total_area << std::setw(12) << total_volume << '\n';
}

/* Автор: Горчак Дмитрий, 212
   Печатает подробные сведения об одной фигуре: строку самого объекта
   (её печатает виртуальный метод print) и вычислимые атрибуты.
   Параметры:
      figure - указатель на фигуру
      number - номер фигуры в таблице
      out    - поток вывода
   Ничего не возвращает. */
void print_one(const FigurePtr &figure, int number, std::ostream &out) {
    const Circle *circle = dynamic_cast<const Circle *>(figure.get());
    const Cone *cone = dynamic_cast<const Cone *>(figure.get());

    out << "  figure " << number << ": ";
    figure->print(out);
    out << '\n';

    if (circle == nullptr) {
        out << "  a point has no area and no volume\n";
        return;
    }
    out << "  base area = " << circle->area() << '\n';
    if (cone == nullptr) {
        out << "  a circle has no volume\n";
        return;
    }
    out << "  volume    = " << cone->volume() << '\n';
    out << "  volume of a pyramid with the same circle around its base:\n";
    out << "    4-gon  = " << cone->volume_ngon(4) << '\n';
    out << "    8-gon  = " << cone->volume_ngon(8) << '\n';
    out << "    64-gon = " << cone->volume_ngon(64) << '\n';
}

/* Автор: Горчак Дмитрий, 212
   Сохраняет данные для вида сверху в три файла: отдельно точки,
   круги и конусы. Так gnuplot рисует каждую фигуру своим стилем.
   Параметры:
      directory - папка для результатов (создаётся, если её нет)
      figures   - вектор фигур
   Возвращает true при успешной записи всех трёх файлов. */
bool save_plan(const std::string &directory, const std::vector<FigurePtr> &figures) {
    std::filesystem::create_directories(directory);

    std::ofstream points((directory + "/plan_points.txt").c_str());
    std::ofstream circles((directory + "/plan_circles.txt").c_str());
    std::ofstream cones((directory + "/plan_cones.txt").c_str());

    if (!points.is_open() || !circles.is_open() || !cones.is_open()) {
        return false;
    }

    points << "# x y\n";
    circles << "# x y r\n";
    cones << "# x y r h\n";
    points << std::fixed << std::setprecision(4);
    circles << std::fixed << std::setprecision(4);
    cones << std::fixed << std::setprecision(4);

    for (std::size_t i = 0; i < figures.size(); ++i) {
        const FigurePtr &figure = figures[i];
        const Circle *circle = dynamic_cast<const Circle *>(figure.get());
        const Cone *cone = dynamic_cast<const Cone *>(figure.get());

        if (cone != nullptr) {
            cones << cone->get_x() << ' ' << cone->get_y() << ' ' << cone->get_r()
                  << ' ' << cone->get_h() << '\n';
        } else if (circle != nullptr) {
            circles << circle->get_x() << ' ' << circle->get_y() << ' '
                    << circle->get_r() << '\n';
        } else {
            points << figure->get_x() << ' ' << figure->get_y() << '\n';
        }
    }

    points.close();
    circles.close();
    cones.close();
    return true;
}

/* Автор: Горчак Дмитрий, 212
   Считает сетку высот z(x, y) внутри границ поля и сохраняет её для
   команды splot. Высоту дают только конусы: высота точки поля -- это
   максимум по всем конусам, поэтому там, где основания пересекаются,
   виден более высокий конус.
   Формат: строки "x y z", после каждого ряда по x -- пустая строка.
   Параметры:
      filename  - имя файла с сеткой
      figures   - вектор фигур
      bounds    - границы поля по x и y
      grid_size - число узлов сетки по каждой оси
   Возвращает true при успешной записи. */
bool save_landscape(const std::string &filename, const std::vector<FigurePtr> &figures,
                    const FieldBounds &bounds, int grid_size) {
    std::ofstream file(filename.c_str());
    float step_x = 0.0f;
    float step_y = 0.0f;

    if (!file.is_open() || grid_size < 2) {
        return false;
    }

    step_x = (bounds.x_max - bounds.x_min) / (grid_size - 1);
    step_y = (bounds.y_max - bounds.y_min) / (grid_size - 1);

    file << "# x y z  (landscape: height of the cone surface)\n";
    file << std::fixed << std::setprecision(4);
    for (int i = 0; i < grid_size; ++i) {
        float x = bounds.x_min + i * step_x;

        for (int j = 0; j < grid_size; ++j) {
            float y = bounds.y_min + j * step_y;
            double z = 0.0;

            for (std::size_t k = 0; k < figures.size(); ++k) {
                const Cone *cone = dynamic_cast<const Cone *>(figures[k].get());
                double current = (cone == nullptr) ? 0.0 : cone->height_at(x, y);

                if (current > z) {
                    z = current;
                }
            }
            file << x << ' ' << y << ' ' << z << '\n';
        }
        file << '\n';                 /* пустая строка -- конец ряда сетки */
    }

    file.close();
    return true;
}

/* Автор: Горчак Дмитрий, 212
   Записывает границы поля командами gnuplot. Скрипт подключает этот
   файл командой load, поэтому размеры рисунка задаёт программа,
   а не человек руками.
   Параметры:
      filename - имя файла со вставкой для gnuplot
      figures  - вектор фигур (нужен для максимальной высоты)
      bounds   - границы поля по x и y
   Возвращает true при успешной записи. */
bool save_range(const std::string &filename, const std::vector<FigurePtr> &figures,
                const FieldBounds &bounds) {
    std::ofstream file(filename.c_str());
    float h_max = 1.0f;

    if (!file.is_open()) {
        return false;
    }

    for (std::size_t i = 0; i < figures.size(); ++i) {
        const Cone *cone = dynamic_cast<const Cone *>(figures[i].get());

        if (cone != nullptr && cone->get_h() > h_max) {
            h_max = cone->get_h();
        }
    }

    file << "# field bounds computed by the program figures\n";
    file << std::fixed << std::setprecision(4);
    file << "set xrange [" << bounds.x_min << ':' << bounds.x_max << "]\n";
    file << "set yrange [" << bounds.y_min << ':' << bounds.y_max << "]\n";
    file << "set zrange [0:" << h_max * 1.1f << "]\n";

    file.close();
    return true;
}

/* Автор: Горчак Дмитрий, 212
   Сохраняет облако точек, лежащих на поверхности фигур: для конуса это
   боковая поверхность, для круга -- окружность основания, для точки --
   она сама. Точки на конусе задаются параметрически: на высоте z радиус
   сечения равен r * (1 - z / h), и по этой окружности расставляются
   точки через равные углы. Такой способ не требует перебора всего
   пространства с проверкой уравнения и даёт ровное покрытие.
   Параметры:
      filename - имя файла с облаком точек
      figures  - вектор фигур
      rings    - число сечений конуса по высоте
      sectors  - число точек на каждой окружности
   Возвращает true при успешной записи. */
bool save_surface_points(const std::string &filename,
                         const std::vector<FigurePtr> &figures, int rings, int sectors) {
    std::ofstream file(filename.c_str());
    const double PI = 3.14159265358979323846;

    if (!file.is_open() || rings < 2 || sectors < 3) {
        return false;
    }

    file << "# x y z  (points on the surface of the figures)\n";
    file << std::fixed << std::setprecision(4);
    for (std::size_t i = 0; i < figures.size(); ++i) {
        const FigurePtr &figure = figures[i];
        const Circle *circle = dynamic_cast<const Circle *>(figure.get());
        const Cone *cone = dynamic_cast<const Cone *>(figure.get());
        double x0 = figure->get_x();
        double y0 = figure->get_y();

        if (circle == nullptr) {                    /* точка -- одна точка */
            file << x0 << ' ' << y0 << " 0.0000\n\n";
            continue;
        }

        double radius = circle->get_r();
        double height = (cone == nullptr) ? 0.0 : cone->get_h();
        int levels = (cone == nullptr) ? 1 : rings; /* у круга одно сечение */

        for (int level = 0; level < levels; ++level) {
            double part = (levels == 1) ? 0.0 : level / static_cast<double>(levels - 1);
            double z = height * part;
            double r = radius * (1.0 - part);       /* радиус сечения на высоте z */

            for (int sector = 0; sector < sectors; ++sector) {
                double angle = 2.0 * PI * sector / sectors;

                file << x0 + r * std::cos(angle) << ' ' << y0 + r * std::sin(angle)
                     << ' ' << z << '\n';
            }
        }
        file << '\n';                               /* пустая строка -- новая фигура */
    }

    file.close();
    return true;
}

/* Автор: Горчак Дмитрий, 212
   Записывает уравнения фигур. Окружность основания радиуса r с центром
   (x0, y0) задаётся уравнением (x - x0)^2 + (y - y0)^2 = r^2, а боковая
   поверхность конуса высоты h -- уравнением
   (x - x0)^2 + (y - y0)^2 = (r - r * z / h)^2, где 0 <= z <= h.
   Параметры:
      filename - имя файла с уравнениями
      figures  - вектор фигур
   Возвращает true при успешной записи. */
bool save_equations(const std::string &filename, const std::vector<FigurePtr> &figures) {
    std::ofstream file(filename.c_str());

    if (!file.is_open()) {
        return false;
    }

    file << "# equations of the figures\n";
    file << std::fixed << std::setprecision(2);
    for (std::size_t i = 0; i < figures.size(); ++i) {
        const FigurePtr &figure = figures[i];
        const Circle *circle = dynamic_cast<const Circle *>(figure.get());
        const Cone *cone = dynamic_cast<const Cone *>(figure.get());

        file << i + 1 << ' ' << figure->type_name() << ": ";
        if (circle == nullptr) {
            file << "x = " << figure->get_x() << ", y = " << figure->get_y() << '\n';
            continue;
        }
        file << "(x - " << circle->get_x() << ")^2 + (y - " << circle->get_y()
             << ")^2 = ";
        if (cone == nullptr) {
            file << circle->get_r() << "^2\n";
        } else {
            file << '(' << cone->get_r() << " - " << cone->get_r() << " * z / "
                 << cone->get_h() << ")^2,  0 <= z <= " << cone->get_h() << '\n';
        }
    }

    file.close();
    return true;
}
