/* gauss_demo.cpp -- иллюстрации к гауссову распределению.
   Программа выдаёт три файла с числами: выборку одномерного
   нормального распределения и два облака двумерного -- без связи
   координат (rho = 0) и со связью (rho = 0.7). По ним gnuplot строит
   гистограмму и точечные диаграммы.
   Автор: Горчак Дмитрий, 212 */

#include <cmath>        /* std::sqrt */
#include <cstdlib>      /* std::atoi */
#include <fstream>      /* std::ofstream */
#include <iomanip>      /* std::fixed, std::setprecision */
#include <iostream>
#include <string>
#include <vector>

#include "gauss.h"

const char *const OUT_DIR = "out";
const int DEFAULT_COUNT = 5000;     /* сколько чисел в выборке */
const unsigned SEED = 212;
const float MU = 0.0f;
const float SIGMA = 1.0f;

bool save_values(const std::string &filename, std::vector<float> &values);
void print_statistics(const std::vector<float> &values);

/* Автор: Горчак Дмитрий, 212
   Точка входа. Строит выборки и сохраняет их для gnuplot.
   Параметры:
      argc - количество аргументов + 1
      argv - argv[1] (необязательный) -- размер выборки
   Возвращает 0 при успехе и -1, если файлы не записались. */
int main(int argc, char *argv[]) {
    int count = DEFAULT_COUNT;
    Gauss gauss(SEED);
    std::vector<float> values;

    if (argc > 2) {
        std::cout << "Usage: " << argv[0] << " [count]\n";
        return -1;
    }
    if (argc == 2) {
        count = std::atoi(argv[1]);
    }
    if (count < 10) {
        std::cout << "Error: count must be >= 10\n";
        return -1;
    }

    std::cout << "\n===============================================================\n";
    std::cout << "Gaussian distribution: one and two dimensions\n";
    std::cout << "===============================================================\n\n";

    /* Пункт А: одномерное распределение N(mu, sigma) */
    std::cout << "--- One-dimensional sample, mu = " << MU << ", sigma = " << SIGMA
              << " ---\n";
    for (int i = 0; i < count; ++i) {
        values.push_back(gauss.next_value(MU, SIGMA));
    }
    print_statistics(values);
    if (!save_values(std::string(OUT_DIR) + "/gauss1d.txt", values)) {
        std::cout << "Error: could not write gauss1d.txt\n";
        return -1;
    }
    std::cout << "  saved: out/gauss1d.txt (" << count << " numbers)\n";

    /* Пункт Б: двумерное распределение без связи и со связью координат */
    for (int step = 0; step < 2; ++step) {
        float rho = (step == 0) ? 0.0f : 0.7f;
        std::string name = (step == 0) ? "/gauss2d_rho00.txt" : "/gauss2d_rho07.txt";
        std::ofstream file((std::string(OUT_DIR) + name).c_str());

        if (!file.is_open()) {
            std::cout << "Error: could not write " << name << '\n';
            return -1;
        }
        file << "# x y   (mu = (0, 0), sigma = (1, 1), rho = " << rho << ")\n";
        file << std::fixed << std::setprecision(4);
        for (int i = 0; i < count / 2; ++i) {
            float x = 0.0f;
            float y = 0.0f;

            gauss.next_point(0.0f, 0.0f, 1.0f, 1.0f, rho, x, y);
            file << x << ' ' << y << '\n';
        }
        file.close();
        std::cout << "  saved: out" << name << " (rho = " << rho << ")\n";
    }

    std::cout << "\nRun 'gnuplot plot_gauss.gp' to draw the pictures.\n";
    std::cout << "Program finished.\n";
    return 0;
}

/* Автор: Горчак Дмитрий, 212
   Записывает выборку чисел в файл по одному числу в строке.
   Параметры:
      filename - имя файла
      values   - выборка
   Возвращает true при успешной записи. */
bool save_values(const std::string &filename, std::vector<float> &values) {
    std::ofstream file(filename.c_str());

    if (!file.is_open()) {
        return false;
    }
    file << "# выборка нормально распределённых чисел\n";
    file << std::fixed << std::setprecision(4);
    for (std::size_t i = 0; i < values.size(); ++i) {
        file << values[i] << '\n';
    }
    file.close();
    return true;
}

/* Автор: Горчак Дмитрий, 212
   Считает по выборке среднее и стандартное отклонение и проверяет
   правило двух сигм: в промежуток mu +- 2*sigma должно попасть
   около 95 процентов чисел.
   Параметры:
      values - выборка
   Ничего не возвращает. */
void print_statistics(const std::vector<float> &values) {
    double sum = 0.0;
    double sum_squares = 0.0;
    double mean = 0.0;
    double deviation = 0.0;
    int inside_one = 0;
    int inside_two = 0;

    for (std::size_t i = 0; i < values.size(); ++i) {
        sum += values[i];
    }
    mean = sum / values.size();

    for (std::size_t i = 0; i < values.size(); ++i) {
        double difference = values[i] - mean;

        sum_squares += difference * difference;
    }
    deviation = std::sqrt(sum_squares / values.size());

    for (std::size_t i = 0; i < values.size(); ++i) {
        double difference = std::fabs(values[i] - mean);

        if (difference <= deviation) {
            ++inside_one;
        }
        if (difference <= 2.0 * deviation) {
            ++inside_two;
        }
    }

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "  sample mean      : " << mean << '\n';
    std::cout << "  sample deviation : " << deviation << '\n';
    std::cout << "  inside 1 sigma   : " << 100.0 * inside_one / values.size()
              << " % (theory 68.3 %)\n";
    std::cout << "  inside 2 sigma   : " << 100.0 * inside_two / values.size()
              << " % (theory 95.4 %)\n";
}
