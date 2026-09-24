/* figures_io.h -- ввод-вывод для фигур: чтение свойств из файла,
   печать таблицы и подготовка данных для gnuplot.
   Сами классы про файлы ничего не знают -- это отдельный модуль.

   В строке файла может быть от двух до четырёх чисел, и от их количества
   зависит, какая фигура будет создана:
      x y       -- точка  (Point)
      x y r     -- круг   (Circle)
      x y r h   -- конус  (Cone)
   Автор: Горчак Дмитрий, 212 */

#ifndef FIGURES_IO_H
#define FIGURES_IO_H

#include <memory>     /* std::shared_ptr -- общий указатель на объект */
#include <ostream>
#include <string>
#include <vector>

#include "cone.h"

/* Указатель на фигуру любого уровня иерархии: Point, Circle или Cone.
   Хранить объекты по указателю на базовый класс нужно для полиморфизма:
   в одном списке лежат разные фигуры, а вызов print у каждой свой. */
typedef std::shared_ptr<Point> FigurePtr;

/* Границы поля по x и y: прямоугольник, в котором лежат все фигуры */
struct FieldBounds {
    float x_min;
    float x_max;
    float y_min;
    float y_max;
};

/* Чтение фигур из текстового файла (в строке 2, 3 или 4 числа);
   false -- файл не открылся. Неверные строки пропускаются с сообщением. */
bool load_figures(const std::string &filename, std::vector<FigurePtr> &figures,
                  std::ostream &log);

/* Площадь основания фигуры: 0 для точки, pi*r*r для круга и конуса */
double area_of(const FigurePtr &figure);

/* Объём фигуры: считается только у конуса, для остальных 0.
   Это и есть «объём по запросу»: у точки и круга его просто нет. */
double volume_of(const FigurePtr &figure);

/* Границы поля с запасом margin_percent процентов от размера */
FieldBounds field_bounds(const std::vector<FigurePtr> &figures, float margin_percent);

/* Таблица свойств: тип фигуры, её числа и вычислимые атрибуты */
void print_table(const std::vector<FigurePtr> &figures, std::ostream &out);

/* Подробная карточка одной фигуры (вызывается по запросу) */
void print_one(const FigurePtr &figure, int number, std::ostream &out);

/* Три файла с данными для вида сверху: точки, круги и конусы отдельно;
   false -- файлы не записались */
bool save_plan(const std::string &directory, const std::vector<FigurePtr> &figures);

/* Сетка высот z(x, y) для ландшафта (splot); высоту дают только конусы */
bool save_landscape(const std::string &filename, const std::vector<FigurePtr> &figures,
                    const FieldBounds &bounds, int grid_size);

/* Границы поля в виде команд gnuplot (set xrange, set yrange, set zrange) */
bool save_range(const std::string &filename, const std::vector<FigurePtr> &figures,
                const FieldBounds &bounds);

/* Облако точек на поверхности фигур для объёмного рисунка точками:
   конус -- боковая поверхность, круг -- окружность основания,
   точка -- она сама. Параметры rings и sectors задают густоту точек. */
bool save_surface_points(const std::string &filename,
                         const std::vector<FigurePtr> &figures, int rings, int sectors);

/* Уравнения фигур в текстовом виде: окружность основания и конус */
bool save_equations(const std::string &filename, const std::vector<FigurePtr> &figures);

#endif
