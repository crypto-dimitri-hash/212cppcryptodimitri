/* list_demo.cpp -- задача «Конусы на плоскости», этап к 26.09:
   конусы складываются в двунаправленный список из STL (std::list)
   по мере создания и упорядочиваются по площади основания и по объёму.
   Готовый шаблон списка взят из библиотеки, свой список не пишем.
   Автор: Горчак Дмитрий, 212 */

#include <iomanip>    /* std::setw, std::fixed, std::setprecision */
#include <iostream>
#include <list>       /* std::list -- шаблон двунаправленного списка */
#include <string>
#include <vector>

#include "cone_io.h"

const char *const DEFAULT_DATA_FILE = "data/cones.txt";
const double SMALL_VOLUME = 20.0;      /* порог для примера с удалением */

bool less_by_area(const Cone &left, const Cone &right);
bool less_by_volume(const Cone &left, const Cone &right);
bool small_volume(const Cone &value);
void print_list(const std::list<Cone> &cones, const std::string &title);
void insert_sorted_by_volume(std::list<Cone> &cones, const Cone &value);

/* Автор: Горчак Дмитрий, 212
   Точка входа. Читает свойства конусов из файла, складывает объекты
   в std::list, упорядочивает список по площади и по объёму, проходит
   его в обе стороны и вставляет новый элемент на своё место.
   Параметры:
      argc - количество аргументов + 1
      argv - argv[1] (необязательный) -- имя файла с данными
   Возвращает 0 при успехе и -1, если файл с данными не прочитан. */
int main(int argc, char *argv[]) {
    std::string data_file = DEFAULT_DATA_FILE;
    std::vector<Cone> loaded;
    std::list<Cone> cones;

    if (argc > 2) {
        std::cout << "Usage: " << argv[0] << " [data_file]\n";
        return -1;
    }
    if (argc == 2) {
        data_file = argv[1];
    }

    std::cout << "\n===============================================================\n";
    std::cout << "Cones in a doubly linked list (STL std::list)\n";
    std::cout << "===============================================================\n\n";

    std::cout << "--- Reading properties from '" << data_file << "' ---\n";
    if (!load_cones(data_file, loaded, std::cout)) {
        std::cout << "Error: could not open '" << data_file << "'.\n";
        return -1;
    }
    if (loaded.empty()) {
        std::cout << "Error: no valid cones in '" << data_file << "'.\n";
        return -1;
    }

    /* Объекты добавляются в список по мере создания: push_back ставит
       новый элемент в конец и связывает его с предыдущим. */
    for (std::size_t i = 0; i < loaded.size(); ++i) {
        cones.push_back(loaded[i]);
    }
    std::cout << "  objects in the list: " << cones.size() << '\n';

    print_list(cones, "As created (order of the file)");

    /* Сортировка списка: у std::list свой метод sort, потому что узлы
       списка нельзя переставлять как элементы массива -- список меняет
       ссылки, а сами объекты остаются на своих местах в памяти. */
    cones.sort(less_by_area);
    print_list(cones, "Sorted by base area");

    cones.sort(less_by_volume);
    print_list(cones, "Sorted by volume");

    /* Двунаправленность: тот же список читается от конца к началу */
    std::cout << "--- Backward walk (volume from largest to smallest) ---\n";
    std::cout << std::fixed << std::setprecision(2);
    for (std::list<Cone>::const_reverse_iterator it = cones.rbegin(); it != cones.rend();
         ++it) {
        std::cout << std::setw(10) << it->volume();
    }
    std::cout << '\n';

    std::cout << "\n--- Head of the list ---\n";
    std::cout << "  n = " << cones.size() << ", first element: ";
    cones.front().print(std::cout);
    std::cout << "\n  last element:  ";
    cones.back().print(std::cout);
    std::cout << '\n';

    /* Вставка в уже упорядоченный список: место ищем проходом,
       сама вставка -- переключение двух ссылок, элементы не сдвигаются. */
    std::cout << "\n--- Insert a new cone keeping the order ---\n";
    insert_sorted_by_volume(cones, Cone(11.0f, 7.0f, 2.0f, 7.0f));
    print_list(cones, "After insertion (still sorted by volume)");

    /* Удаление по условию: remove_if убирает узлы, не трогая остальные */
    std::cout << "--- Remove cones with volume < " << SMALL_VOLUME << " ---\n";
    cones.remove_if(small_volume);
    print_list(cones, "After removal");

    std::cout << "Program finished.\n";
    return 0;
}

/* Автор: Горчак Дмитрий, 212
   Сравнение двух конусов по площади основания.
   Параметры:
      left  - первый конус
      right - второй конус
   Возвращает true, если площадь основания left меньше. */
bool less_by_area(const Cone &left, const Cone &right) {
    return left.area() < right.area();
}

/* Автор: Горчак Дмитрий, 212
   Сравнение двух конусов по объёму.
   Параметры:
      left  - первый конус
      right - второй конус
   Возвращает true, если объём left меньше. */
bool less_by_volume(const Cone &left, const Cone &right) {
    return left.volume() < right.volume();
}

/* Автор: Горчак Дмитрий, 212
   Условие для remove_if: конус считается маленьким, если его объём
   меньше порога SMALL_VOLUME.
   Параметры:
      value - проверяемый конус
   Возвращает true, если конус нужно удалить из списка. */
bool small_volume(const Cone &value) {
    return value.volume() < SMALL_VOLUME;
}

/* Автор: Горчак Дмитрий, 212
   Печатает список в виде таблицы: номер по порядку, четыре хранимых
   числа и два вычислимых атрибута. По списку идём итератором --
   обращения по индексу у списка нет.
   Параметры:
      cones - список конусов
      title - заголовок таблицы
   Ничего не возвращает. */
void print_list(const std::list<Cone> &cones, const std::string &title) {
    int number = 1;

    std::cout << "\n--- " << title << " ---\n";
    std::cout << std::setw(4) << "N" << std::setw(10) << "x" << std::setw(10) << "y"
              << std::setw(10) << "r" << std::setw(10) << "h" << std::setw(14) << "area"
              << std::setw(14) << "volume" << '\n';
    std::cout << std::string(72, '-') << '\n';
    std::cout << std::fixed << std::setprecision(2);
    for (std::list<Cone>::const_iterator it = cones.begin(); it != cones.end(); ++it) {
        std::cout << std::setw(4) << number << std::setw(10) << it->get_x()
                  << std::setw(10) << it->get_y() << std::setw(10) << it->get_r()
                  << std::setw(10) << it->get_h() << std::setw(14) << it->area()
                  << std::setw(14) << it->volume() << '\n';
        ++number;
    }
    std::cout << '\n';
}

/* Автор: Горчак Дмитрий, 212
   Вставляет конус в список, упорядоченный по объёму, так, чтобы порядок
   сохранился: идём от начала до первого элемента с большим объёмом
   и вставляем перед ним.
   Параметры:
      cones - список, упорядоченный по объёму
      value - новый конус
   Ничего не возвращает. */
void insert_sorted_by_volume(std::list<Cone> &cones, const Cone &value) {
    std::list<Cone>::iterator place = cones.begin();

    while (place != cones.end() && place->volume() < value.volume()) {
        ++place;
    }
    cones.insert(place, value);      /* insert ставит элемент перед place */

    std::cout << "  inserted: ";
    value.print(std::cout);
    std::cout << '\n';
}
