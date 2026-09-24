/* main.cpp -- фигуры на плоскости: иерархия Point -> Circle -> Cone.
   Программа читает из файла свойства фигур (в строке от двух до четырёх
   чисел), создаёт точку, круг или конус в зависимости от количества
   чисел, печатает таблицу и готовит данные для gnuplot.
   Объём считается отдельно, по запросу: номер нужной фигуры передаётся
   вторым аргументом командной строки.
   Автор: Горчак Дмитрий, 212 */

#include <cstdlib>      /* std::atoi */
#include <iostream>
#include <stdexcept>
#include <filesystem>     /* std::filesystem::create_directories */
#include <string>
#include <vector>

#include "figures_io.h"

const char *const DEFAULT_DATA_FILE = "figures.txt";
const char *const OUT_DIR = "out";                  /* папка для результатов */
const int GRID_SIZE = 121;                          /* узлов сетки по каждой оси */
const float MARGIN_PERCENT = 8.0f;                  /* запас поля вокруг фигур */
const int RINGS = 24;                               /* сечений конуса по высоте */
const int SECTORS = 48;                             /* точек на каждой окружности */

void show_hierarchy();
void show_encapsulation();

/* Автор: Горчак Дмитрий, 212
   Точка входа. Читает свойства фигур из файла, печатает таблицу,
   по запросу считает объём одной фигуры, определяет границы поля
   и сохраняет данные для gnuplot в папку out.
   Параметры:
      argc - количество аргументов + 1
      argv - argv[1] (необязательный) -- имя файла с данными,
             argv[2] (необязательный) -- номер фигуры для подробного вывода
   Возвращает 0 при успехе и -1 при ошибке. */
int main(int argc, char *argv[]) {
    std::filesystem::create_directories(OUT_DIR);   /* папка для результатов */
    std::string data_file = DEFAULT_DATA_FILE;
    std::vector<FigurePtr> figures;
    FieldBounds bounds = {0.0f, 0.0f, 0.0f, 0.0f};
    int wanted = 0;

    if (argc > 3) {
        std::cout << "Usage: ./figures [data_file] [figure_number]\n";
        return -1;
    }
    if (argc >= 2) {
        data_file = argv[1];
    }
    if (argc == 3) {
        wanted = std::atoi(argv[2]);
    }

    std::cout << "\n===============================================================\n";
    std::cout << "Figures on a plane: Point -> Circle -> Cone\n";
    std::cout << "2 numbers make a point, 3 a circle, 4 a cone\n";
    std::cout << "===============================================================\n\n";

    show_hierarchy();
    show_encapsulation();

    std::cout << "\n--- Reading properties from '" << data_file << "' ---\n";
    if (!load_figures(data_file, figures, std::cout)) {
        std::cout << "Error: could not open '" << data_file << "'.\n";
        return -1;
    }
    if (figures.empty()) {
        std::cout << "Error: no valid figures in '" << data_file << "'.\n";
        return -1;
    }
    std::cout << "  objects created: " << figures.size() << '\n';

    std::cout << "\n--- Stored properties and computed attributes ---\n";
    print_table(figures, std::cout);

    /* Объём по запросу: печатается только для указанной фигуры */
    if (wanted > 0) {
        std::cout << "\n--- Figure asked on the command line ---\n";
        if (wanted > static_cast<int>(figures.size())) {
            std::cout << "  there is no figure number " << wanted << '\n';
        } else {
            print_one(figures[wanted - 1], wanted, std::cout);
        }
    } else {
        std::cout << "\nAsk for one figure: ./figures " << data_file
                  << " 5   (area and volume of figure 5)\n";
    }

    bounds = field_bounds(figures, MARGIN_PERCENT);
    std::cout << "\n--- Field bounds ---\n";
    std::cout << "  x: [" << bounds.x_min << "; " << bounds.x_max << "]\n";
    std::cout << "  y: [" << bounds.y_min << "; " << bounds.y_max << "]\n";

    std::cout << "\n--- Data for gnuplot (folder '" << OUT_DIR << "') ---\n";
    if (!save_plan(OUT_DIR, figures)) {
        std::cout << "Warning: could not write the top view files.\n";
    } else {
        std::cout << "  plan_points.txt, plan_circles.txt, plan_cones.txt -- top view\n";
    }
    if (!save_landscape(std::string(OUT_DIR) + "/field.txt", figures, bounds, GRID_SIZE)) {
        std::cout << "Warning: could not write the landscape grid.\n";
    } else {
        std::cout << "  field.txt -- landscape grid " << GRID_SIZE << " x " << GRID_SIZE
                  << '\n';
    }
    if (!save_range(std::string(OUT_DIR) + "/range.gp", figures, bounds)) {
        std::cout << "Warning: could not write the field bounds.\n";
    } else {
        std::cout << "  range.gp -- field bounds for gnuplot\n";
    }
    if (!save_surface_points(std::string(OUT_DIR) + "/surface_points.txt", figures,
                             RINGS, SECTORS)) {
        std::cout << "Warning: could not write the surface points.\n";
    } else {
        std::cout << "  surface_points.txt -- points on the surface of every figure\n";
    }
    if (!save_equations(std::string(OUT_DIR) + "/equations.txt", figures)) {
        std::cout << "Warning: could not write the equations.\n";
    } else {
        std::cout << "  equations.txt -- equation of every figure\n";
    }

    std::cout << "\nRun 'gnuplot plot_figures.gp' to draw the field.\n";
    std::cout << "Program finished.\n";
    return 0;
}

/* Автор: Горчак Дмитрий, 212
   Показывает иерархию классов: точка, круг и конус выводятся через
   указатель на общий базовый класс Point. Вызов один и тот же, а печатает
   каждый объект по-своему -- это полиморфизм (метод print виртуальный).
   Параметров нет.
   Ничего не возвращает. */
void show_hierarchy() {
    Point point(1.0f, 2.0f);
    Circle circle(1.0f, 2.0f, 3.0f);
    Cone cone(1.0f, 2.0f, 3.0f, 4.0f);
    const Point *figures[] = {&point, &circle, &cone};
    const int count = 3;

    std::cout << "--- Class hierarchy (one call, different output) ---\n";
    for (int i = 0; i < count; ++i) {
        std::cout << "  ";
        figures[i]->print(std::cout);
        std::cout << '\n';
    }
    std::cout << "  a point needs 2 numbers, a circle 3, a cone 4\n";
}

/* Автор: Горчак Дмитрий, 212
   Показывает работу инкапсуляции: поля закрыты, читать и менять их можно
   только через get/set, а проверка значений живёт внутри set-методов,
   поэтому создать конус с отрицательным радиусом нельзя.
   Параметров нет.
   Ничего не возвращает. */
void show_encapsulation() {
    Cone cone(0.0f, 0.0f, 2.0f, 6.0f);

    std::cout << "\n--- Private fields, access through get/set ---\n";
    std::cout << "  get_r() = " << cone.get_r() << ", get_h() = " << cone.get_h()
              << ", volume() = " << cone.volume() << '\n';

    cone.set_r(4.0f);                 /* меняем радиус -- объём пересчитывается сам */
    std::cout << "  after set_r(4): volume() = " << cone.volume()
              << " (the value is not stored, it is computed)\n";

    try {
        cone.set_r(-1.0f);            /* так поле испортить не получится */
        std::cout << "  radius -1 accepted (this line must not appear)\n";
    } catch (const std::invalid_argument &error) {
        std::cout << "  rejected: " << error.what() << '\n';
    }
    try {
        Cone wrong(0.0f, 0.0f, 1.0f, -5.0f);
        std::cout << "  height -5 accepted: " << wrong.get_h() << '\n';
    } catch (const std::invalid_argument &error) {
        std::cout << "  rejected: " << error.what() << '\n';
    }
}
