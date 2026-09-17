/* cone_io.h -- ввод-вывод для конусов: чтение свойств из файла,
   печать таблицы и подготовка данных для gnuplot.
   Сами классы про файлы ничего не знают -- это отдельный модуль
   (раздельная компиляция, занятие 5).
   Автор: Горчак Дмитрий, 212 */

#ifndef CONE_IO_H
#define CONE_IO_H

#include <ostream>
#include <string>
#include <vector>

#include "cone.h"

/* Границы поля по x и y: прямоугольник, в котором лежат все конусы */
struct FieldBounds {
    float x_min;
    float x_max;
    float y_min;
    float y_max;
};

/* Чтение конусов из текстового файла (строка: x y r h);
   false -- файл не открылся. Неверные строки пропускаются с сообщением. */
bool load_cones(const std::string &filename, std::vector<Cone> &cones, std::ostream &log);

/* Границы поля с запасом margin_percent процентов от размера */
FieldBounds field_bounds(const std::vector<Cone> &cones, float margin_percent);

/* Таблица свойств: четыре числа, площадь основания и объём */
void print_table(const std::vector<Cone> &cones, std::ostream &out);

/* Данные для вида сверху: x y r h area volume; false -- файл не записался */
bool save_plan(const std::string &filename, const std::vector<Cone> &cones);

/* Сетка высот z(x, y) для ландшафта (splot); false -- файл не записался */
bool save_landscape(const std::string &filename, const std::vector<Cone> &cones,
                    const FieldBounds &bounds, int grid_size);

/* Границы поля в виде команд gnuplot (set xrange, set yrange, set zrange) */
bool save_range(const std::string &filename, const std::vector<Cone> &cones,
                const FieldBounds &bounds);

#endif
