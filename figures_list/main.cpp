/* list_demo.cpp -- двунаправленный список из STL (std::list).
   Фигуры складываются в список по мере создания и упорядочиваются
   по площади основания и по объёму. Готовый шаблон списка взят из
   библиотеки, свой список не пишем.
   Автор: Горчак Дмитрий, 212 */

#include <iomanip>    /* std::setw, std::fixed, std::setprecision */
#include <iostream>
#include <list>       /* std::list -- шаблон двунаправленного списка */
#include <string>
#include <vector>

#include "figures_io.h"

const char *const DEFAULT_DATA_FILE = "figures.txt";
const double SMALL_VOLUME = 20.0;      /* порог для примера с удалением */

bool less_by_area(const FigurePtr &left, const FigurePtr &right);
bool less_by_volume(const FigurePtr &left, const FigurePtr &right);
bool small_volume(const FigurePtr &value);
void print_list(const std::list<FigurePtr> &figures, const std::string &title);
void insert_sorted_by_volume(std::list<FigurePtr> &figures, const FigurePtr &value);

/* Автор: Горчак Дмитрий, 212
   Точка входа. Читает фигуры из файла, складывает их в std::list,
   упорядочивает по площади и по объёму, проходит список в обе стороны
   и вставляет новый элемент на своё место.
   Параметры:
      argc - количество аргументов + 1
      argv - argv[1] (необязательный) -- имя файла с данными
   Возвращает 0 при успехе и -1, если файл с данными не прочитан. */
int main(int argc, char *argv[]) {
    std::string data_file = DEFAULT_DATA_FILE;
    std::vector<FigurePtr> loaded;
    std::list<FigurePtr> figures;

    if (argc > 2) {
        std::cout << "Usage: ./figures_list [data_file]\n";
        return -1;
    }
    if (argc == 2) {
        data_file = argv[1];
    }

    std::cout << "\n===============================================================\n";
    std::cout << "Figures in a doubly linked list (STL std::list)\n";
    std::cout << "===============================================================\n\n";

    std::cout << "--- Reading properties from '" << data_file << "' ---\n";
    if (!load_figures(data_file, loaded, std::cout)) {
        std::cout << "Error: could not open '" << data_file << "'.\n";
        return -1;
    }
    if (loaded.empty()) {
        std::cout << "Error: no valid figures in '" << data_file << "'.\n";
        return -1;
    }

    /* Объекты добавляются в список по мере создания: push_back ставит
       новый элемент в конец и связывает его с предыдущим. */
    for (std::size_t i = 0; i < loaded.size(); ++i) {
        figures.push_back(loaded[i]);
    }
    std::cout << "  objects in the list: " << figures.size() << '\n';

    print_list(figures, "As created (order of the file)");

    /* Сортировка списка: у std::list свой метод sort, потому что узлы
       списка нельзя переставлять как элементы массива -- список меняет
       ссылки, а сами объекты остаются на своих местах в памяти. */
    figures.sort(less_by_area);
    print_list(figures, "Sorted by base area");

    figures.sort(less_by_volume);
    print_list(figures, "Sorted by volume");

    /* Двунаправленность: тот же список читается от конца к началу */
    std::cout << "--- Backward walk (volume from largest to smallest) ---\n";
    std::cout << std::fixed << std::setprecision(2);
    for (std::list<FigurePtr>::const_reverse_iterator it = figures.rbegin();
         it != figures.rend(); ++it) {
        std::cout << std::setw(9) << volume_of(*it);
    }
    std::cout << '\n';

    std::cout << "\n--- Head of the list ---\n";
    std::cout << "  n = " << figures.size() << ", first element: ";
    figures.front()->print(std::cout);
    std::cout << "\n  last element:  ";
    figures.back()->print(std::cout);
    std::cout << '\n';

    /* Вставка в уже упорядоченный список: место ищем проходом,
       сама вставка -- переключение двух ссылок, элементы не сдвигаются. */
    std::cout << "\n--- Insert a new cone keeping the order ---\n";
    insert_sorted_by_volume(figures, FigurePtr(new Cone(11.0f, 7.0f, 2.0f, 7.0f)));
    print_list(figures, "After insertion (still sorted by volume)");

    /* Удаление по условию: remove_if убирает узлы, не трогая остальные */
    std::cout << "--- Remove figures with volume < " << SMALL_VOLUME << " ---\n";
    figures.remove_if(small_volume);
    print_list(figures, "After removal");

    std::cout << "Program finished.\n";
    return 0;
}

/* Автор: Горчак Дмитрий, 212
   Сравнение двух фигур по площади основания (у точки она равна нулю).
   Параметры:
      left  - первая фигура
      right - вторая фигура
   Возвращает true, если площадь основания left меньше. */
bool less_by_area(const FigurePtr &left, const FigurePtr &right) {
    return area_of(left) < area_of(right);
}

/* Автор: Горчак Дмитрий, 212
   Сравнение двух фигур по объёму (он есть только у конуса).
   Параметры:
      left  - первая фигура
      right - вторая фигура
   Возвращает true, если объём left меньше. */
bool less_by_volume(const FigurePtr &left, const FigurePtr &right) {
    return volume_of(left) < volume_of(right);
}

/* Автор: Горчак Дмитрий, 212
   Условие для remove_if: фигура считается маленькой, если её объём
   меньше порога SMALL_VOLUME.
   Параметры:
      value - проверяемая фигура
   Возвращает true, если фигуру нужно удалить из списка. */
bool small_volume(const FigurePtr &value) {
    return volume_of(value) < SMALL_VOLUME;
}

/* Автор: Горчак Дмитрий, 212
   Печатает список в виде таблицы: номер по порядку, тип фигуры, её
   числа и вычислимые атрибуты. По списку идём итератором -- обращения
   по индексу у списка нет.
   Параметры:
      figures - список фигур
      title   - заголовок таблицы
   Ничего не возвращает. */
void print_list(const std::list<FigurePtr> &figures, const std::string &title) {
    std::vector<FigurePtr> copy;

    std::cout << "\n--- " << title << " ---\n";
    for (std::list<FigurePtr>::const_iterator it = figures.begin(); it != figures.end();
         ++it) {
        copy.push_back(*it);            /* таблицу печатает общий модуль */
    }
    print_table(copy, std::cout);
    std::cout << '\n';
}

/* Автор: Горчак Дмитрий, 212
   Вставляет фигуру в список, упорядоченный по объёму, так, чтобы
   порядок сохранился: идём от начала до первого элемента с большим
   объёмом и вставляем перед ним.
   Параметры:
      figures - список, упорядоченный по объёму
      value   - новая фигура
   Ничего не возвращает. */
void insert_sorted_by_volume(std::list<FigurePtr> &figures, const FigurePtr &value) {
    std::list<FigurePtr>::iterator place = figures.begin();

    while (place != figures.end() && volume_of(*place) < volume_of(value)) {
        ++place;
    }
    figures.insert(place, value);      /* insert ставит элемент перед place */

    std::cout << "  inserted: ";
    value->print(std::cout);
    std::cout << '\n';
}
