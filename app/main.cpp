/* main.cpp -- задача «Конусы на плоскости», этап к 19.09:
   иерархия классов Point -> Circle -> Cone, создание объектов по
   свойствам из файла, вычислимые атрибуты (площадь, объём) и подготовка
   данных для gnuplot (вид сверху и ландшафт с границами по x и y).
   Объекты в список пока не записываются -- это следующий этап (26.09).
   Автор: Горчак Дмитрий, 212 */

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "cone_io.h"

const char *const DEFAULT_DATA_FILE = "data/cones.txt";
const char *const PLAN_FILE = "cones_plan.txt";       /* вид сверху */
const char *const FIELD_FILE = "cones_field.txt";     /* сетка высот */
const char *const RANGE_FILE = "cones_range.gp";      /* границы поля для gnuplot */
const int GRID_SIZE = 121;                            /* узлов сетки по каждой оси */
const float MARGIN_PERCENT = 8.0f;                    /* запас поля вокруг конусов */

void show_hierarchy();
void show_encapsulation();

/* Автор: Горчак Дмитрий, 212
   Точка входа. Читает свойства конусов из файла, печатает таблицу,
   считает границы поля и сохраняет три файла для gnuplot.
   Параметры:
      argc - количество аргументов + 1
      argv - argv[1] (необязательный) -- имя файла с данными
   Возвращает 0 при успехе и -1, если файл с данными не прочитан. */
int main(int argc, char *argv[]) {
    std::string data_file = DEFAULT_DATA_FILE;
    std::vector<Cone> cones;
    FieldBounds bounds = {0.0f, 0.0f, 0.0f, 0.0f};

    if (argc > 2) {
        std::cout << "Usage: " << argv[0] << " [data_file]\n";
        return -1;
    }
    if (argc == 2) {
        data_file = argv[1];
    }

    std::cout << "\n===============================================================\n";
    std::cout << "Cones on a plane: Point -> Circle -> Cone\n";
    std::cout << "===============================================================\n\n";

    show_hierarchy();
    show_encapsulation();

    std::cout << "\n--- Reading properties from '" << data_file << "' ---\n";
    if (!load_cones(data_file, cones, std::cout)) {
        std::cout << "Error: could not open '" << data_file << "'.\n";
        return -1;
    }
    if (cones.empty()) {
        std::cout << "Error: no valid cones in '" << data_file << "'.\n";
        return -1;
    }
    std::cout << "  objects created: " << cones.size() << '\n';

    std::cout << "\n--- Stored properties and computed attributes ---\n";
    print_table(cones, std::cout);

    /* Численная разница между конусом и пирамидой: одно и то же основание,
       вписанный правильный n-угольник против круга. */
    std::cout << "\n--- Cone vs pyramid inscribed in the same circle (object 1) ---\n";
    std::cout << "  round base (cone)   : " << cones[0].volume() << '\n';
    std::cout << "  4-gon base          : " << cones[0].volume_ngon(4) << '\n';
    std::cout << "  8-gon base          : " << cones[0].volume_ngon(8) << '\n';
    std::cout << "  64-gon base         : " << cones[0].volume_ngon(64) << '\n';

    bounds = field_bounds(cones, MARGIN_PERCENT);
    std::cout << "\n--- Field bounds ---\n";
    std::cout << "  x: [" << bounds.x_min << "; " << bounds.x_max << "]\n";
    std::cout << "  y: [" << bounds.y_min << "; " << bounds.y_max << "]\n";

    std::cout << "\n--- Data for gnuplot ---\n";
    if (!save_plan(PLAN_FILE, cones)) {
        std::cout << "Warning: could not write '" << PLAN_FILE << "'.\n";
    } else {
        std::cout << "  " << PLAN_FILE << " -- top view (circles)\n";
    }
    if (!save_landscape(FIELD_FILE, cones, bounds, GRID_SIZE)) {
        std::cout << "Warning: could not write '" << FIELD_FILE << "'.\n";
    } else {
        std::cout << "  " << FIELD_FILE << " -- landscape grid " << GRID_SIZE << " x "
                  << GRID_SIZE << '\n';
    }
    if (!save_range(RANGE_FILE, cones, bounds)) {
        std::cout << "Warning: could not write '" << RANGE_FILE << "'.\n";
    } else {
        std::cout << "  " << RANGE_FILE << " -- field bounds for gnuplot\n";
    }

    std::cout << "\nRun 'gnuplot plot/plot_cones.gp' to draw the field.\n";
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
    std::cout << "  a cone is described by four numbers: x, y, r, h\n";
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
