/* cone_io.cpp -- реализация ввода-вывода для конусов.
   Автор: Горчак Дмитрий, 212 */

#include "cone_io.h"

#include <fstream>      /* std::ifstream, std::ofstream */
#include <iomanip>      /* std::setw, std::fixed, std::setprecision */
#include <sstream>      /* std::istringstream -- разбор одной строки файла */
#include <stdexcept>    /* std::invalid_argument */

/* Автор: Горчак Дмитрий, 212
   Читает конусы из текстового файла. Формат строки -- четыре числа:
   x y r h. Пустые строки и строки, начинающиеся с '#', пропускаются.
   Строка с неверными данными не прерывает чтение: о ней сообщается в log,
   и разбор продолжается со следующей строки.
   Параметры:
      filename - имя файла с исходными данными
      cones    - вектор, в который добавляются прочитанные конусы
      log      - поток для сообщений об ошибочных строках
   Возвращает true, если файл открылся, и false, если его нет. */
bool load_cones(const std::string &filename, std::vector<Cone> &cones, std::ostream &log) {
    std::ifstream file(filename.c_str());
    std::string line;
    int line_number = 0;

    if (!file.is_open()) {
        return false;
    }

    while (std::getline(file, line)) {
        float x = 0.0f;
        float y = 0.0f;
        float r = 0.0f;
        float h = 0.0f;

        ++line_number;
        if (line.empty() || line[0] == '#') {
            continue;                       /* комментарий или пустая строка */
        }

        std::istringstream input(line);
        if (!(input >> x >> y >> r >> h)) {
            log << "  line " << line_number << " skipped: need 4 numbers (x y r h)\n";
            continue;
        }

        /* Проверка значений спрятана в set-методах классов, поэтому
           здесь достаточно перехватить исключение. */
        try {
            cones.push_back(Cone(x, y, r, h));
        } catch (const std::invalid_argument &error) {
            log << "  line " << line_number << " skipped: " << error.what() << '\n';
        }
    }

    file.close();
    return true;
}

/* Автор: Горчак Дмитрий, 212
   Определяет границы поля по x и y: берёт крайние точки оснований
   (центр плюс-минус радиус) и добавляет запас, чтобы круги не упирались
   в рамку рисунка.
   Параметры:
      cones          - вектор конусов
      margin_percent - запас в процентах от размера поля
   Возвращает структуру с границами; для пустого вектора -- нулевой квадрат. */
FieldBounds field_bounds(const std::vector<Cone> &cones, float margin_percent) {
    FieldBounds bounds = {0.0f, 0.0f, 0.0f, 0.0f};
    float margin = 0.0f;

    if (cones.empty()) {
        return bounds;
    }

    bounds.x_min = cones[0].get_x() - cones[0].get_r();
    bounds.x_max = cones[0].get_x() + cones[0].get_r();
    bounds.y_min = cones[0].get_y() - cones[0].get_r();
    bounds.y_max = cones[0].get_y() + cones[0].get_r();

    for (std::size_t i = 1; i < cones.size(); ++i) {
        const Cone &c = cones[i];

        if (c.get_x() - c.get_r() < bounds.x_min) {
            bounds.x_min = c.get_x() - c.get_r();
        }
        if (c.get_x() + c.get_r() > bounds.x_max) {
            bounds.x_max = c.get_x() + c.get_r();
        }
        if (c.get_y() - c.get_r() < bounds.y_min) {
            bounds.y_min = c.get_y() - c.get_r();
        }
        if (c.get_y() + c.get_r() > bounds.y_max) {
            bounds.y_max = c.get_y() + c.get_r();
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
   Печатает таблицу конусов: номер, четыре хранимых числа и два
   вычислимых атрибута -- площадь основания и объём. В конце -- итоги.
   Параметры:
      cones - вектор конусов
      out   - поток вывода (экран или файл)
   Ничего не возвращает. */
void print_table(const std::vector<Cone> &cones, std::ostream &out) {
    double total_area = 0.0;
    double total_volume = 0.0;

    out << std::setw(4) << "N" << std::setw(10) << "x" << std::setw(10) << "y"
        << std::setw(10) << "r" << std::setw(10) << "h" << std::setw(14) << "area"
        << std::setw(14) << "volume" << '\n';
    out << std::string(72, '-') << '\n';

    out << std::fixed << std::setprecision(2);
    for (std::size_t i = 0; i < cones.size(); ++i) {
        const Cone &c = cones[i];

        out << std::setw(4) << i + 1 << std::setw(10) << c.get_x() << std::setw(10)
            << c.get_y() << std::setw(10) << c.get_r() << std::setw(10) << c.get_h()
            << std::setw(14) << c.area() << std::setw(14) << c.volume() << '\n';
        total_area += c.area();
        total_volume += c.volume();
    }

    out << std::string(72, '-') << '\n';
    out << std::setw(4) << cones.size() << std::setw(40) << "total:" << std::setw(14)
        << total_area << std::setw(14) << total_volume << '\n';
}

/* Автор: Горчак Дмитрий, 212
   Сохраняет свойства конусов для вида сверху: gnuplot рисует по этому
   файлу круги оснований и подписи.
   Параметры:
      filename - имя файла с данными
      cones    - вектор конусов
   Возвращает true при успешной записи. */
bool save_plan(const std::string &filename, const std::vector<Cone> &cones) {
    std::ofstream file(filename.c_str());

    if (!file.is_open()) {
        return false;
    }

    file << "# x y r h area volume  (данные для вида сверху)\n";
    file << std::fixed << std::setprecision(4);
    for (std::size_t i = 0; i < cones.size(); ++i) {
        const Cone &c = cones[i];

        file << c.get_x() << ' ' << c.get_y() << ' ' << c.get_r() << ' ' << c.get_h()
             << ' ' << c.area() << ' ' << c.volume() << '\n';
    }

    file.close();
    return true;
}

/* Автор: Горчак Дмитрий, 212
   Считает сетку высот z(x, y) внутри границ поля и сохраняет её для
   команды splot. Высота точки -- максимум по всем конусам, поэтому
   в местах пересечения оснований виден более высокий конус.
   Формат: строки "x y z", после каждого ряда по x -- пустая строка
   (так gnuplot понимает, что данные образуют сетку).
   Параметры:
      filename  - имя файла с сеткой
      cones     - вектор конусов
      bounds    - границы поля по x и y
      grid_size - число узлов сетки по каждой оси
   Возвращает true при успешной записи. */
bool save_landscape(const std::string &filename, const std::vector<Cone> &cones,
                    const FieldBounds &bounds, int grid_size) {
    std::ofstream file(filename.c_str());
    float step_x = 0.0f;
    float step_y = 0.0f;

    if (!file.is_open() || grid_size < 2) {
        return false;
    }

    step_x = (bounds.x_max - bounds.x_min) / (grid_size - 1);
    step_y = (bounds.y_max - bounds.y_min) / (grid_size - 1);

    file << "# x y z  (ландшафт: высота поверхности конусов над плоскостью)\n";
    file << std::fixed << std::setprecision(4);
    for (int i = 0; i < grid_size; ++i) {
        float x = bounds.x_min + i * step_x;

        for (int j = 0; j < grid_size; ++j) {
            float y = bounds.y_min + j * step_y;
            double z = 0.0;

            for (std::size_t k = 0; k < cones.size(); ++k) {
                double current = cones[k].height_at(x, y);

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
      cones    - вектор конусов (нужен для максимальной высоты)
      bounds   - границы поля по x и y
   Возвращает true при успешной записи. */
bool save_range(const std::string &filename, const std::vector<Cone> &cones,
                const FieldBounds &bounds) {
    std::ofstream file(filename.c_str());
    float h_max = 0.0f;

    if (!file.is_open()) {
        return false;
    }

    for (std::size_t i = 0; i < cones.size(); ++i) {
        if (cones[i].get_h() > h_max) {
            h_max = cones[i].get_h();
        }
    }

    file << "# границы поля, посчитанные программой cones_app\n";
    file << std::fixed << std::setprecision(4);
    file << "set xrange [" << bounds.x_min << ':' << bounds.x_max << "]\n";
    file << "set yrange [" << bounds.y_min << ':' << bounds.y_max << "]\n";
    file << "set zrange [0:" << h_max * 1.1f << "]\n";

    file.close();
    return true;
}
