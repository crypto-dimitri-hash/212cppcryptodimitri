/* gauss_demo.cpp -- иллюстрации к гауссову распределению.
   Программа выдаёт выборку одномерного нормального распределения и
   шесть облаков двумерного: круглое, вытянутое вдоль оси абсцисс,
   вытянутое вдоль оси ординат и три облака со связью координат --
   положительной, отрицательной и почти полной. По этим файлам gnuplot
   строит гистограмму и точечные диаграммы.
   Автор: Горчак Дмитрий, 212 */

#include <cmath>        /* std::sqrt */
#include <cstdlib>      /* std::atoi */
#include <fstream>      /* std::ofstream */
#include <iomanip>      /* std::fixed, std::setprecision */
#include <iostream>
#include <filesystem>     /* std::filesystem::create_directories */
#include <string>
#include <vector>

#include "gauss.h"

const char *const OUT_DIR = "out";
const int DEFAULT_COUNT = 5000;     /* сколько чисел в выборке */
const unsigned SEED = 212;
const float MU = 0.0f;
const float SIGMA = 1.0f;

/* Описание одного двумерного облака: имя файла, разбросы по осям и
   коэффициент связи координат. Центр у всех облаков в начале координат,
   поэтому отдельно он не хранится. */
struct Cloud {
    const char *name;       /* имя файла в папке out */
    float sigma_x;          /* разброс вдоль оси абсцисс */
    float sigma_y;          /* разброс вдоль оси ординат */
    float rho;              /* связь координат, от -1 до 1 */
    const char *comment;    /* пояснение для вывода на экран */
};

/* Шесть случаев: сначала связи нет и форму задают только разбросы,
   потом разбросы одинаковы и форму задаёт только связь. */
const Cloud CLOUDS[] = {
    {"gauss2d_round.txt",  1.0f, 1.0f,  0.0f,  "rho = 0, equal sigmas: round cloud"},
    {"gauss2d_wide.txt",   2.0f, 0.6f,  0.0f,  "rho = 0, sigma_x is larger: along the x axis"},
    {"gauss2d_tall.txt",   0.6f, 2.0f,  0.0f,  "rho = 0, sigma_y is larger: along the y axis"},
    {"gauss2d_rho07.txt",  1.0f, 1.0f,  0.7f,  "rho = 0.7: tilted to the right"},
    {"gauss2d_rho-07.txt", 1.0f, 1.0f, -0.7f,  "rho = -0.7: tilted to the left"},
    {"gauss2d_rho095.txt", 1.0f, 1.0f,  0.95f, "rho = 0.95: almost a straight line"}
};
const int CLOUD_COUNT = sizeof(CLOUDS) / sizeof(CLOUDS[0]);

bool save_values(const std::string &filename, std::vector<float> &values);
bool save_cloud(const Cloud &cloud, Gauss &gauss, int count);
void print_statistics(const std::vector<float> &values);

/* Автор: Горчак Дмитрий, 212
   Точка входа. Строит выборки и сохраняет их для gnuplot.
   Параметры:
      argc - количество аргументов + 1
      argv - argv[1] (необязательный) -- размер выборки
   Возвращает 0 при успехе и -1, если файлы не записались. */
int main(int argc, char *argv[]) {
    std::filesystem::create_directories(OUT_DIR);   /* папка для результатов */
    int count = DEFAULT_COUNT;
    Gauss gauss(SEED);
    std::vector<float> values;

    if (argc > 2) {
        std::cout << "Usage: ./gauss_demo [count]\n";
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

    /* Пункт Б: двумерные облака -- форма от разбросов и от связи координат */
    std::cout << "\n--- Two-dimensional clouds, mu = (0, 0) ---\n";
    for (int i = 0; i < CLOUD_COUNT; ++i) {
        if (!save_cloud(CLOUDS[i], gauss, count / 2)) {
            std::cout << "Error: could not write " << CLOUDS[i].name << '\n';
            return -1;
        }
        std::cout << "  saved: out/" << CLOUDS[i].name << " -- "
                  << CLOUDS[i].comment << '\n';
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
    file << "# sample of normally distributed numbers\n";
    file << std::fixed << std::setprecision(4);
    for (std::size_t i = 0; i < values.size(); ++i) {
        file << values[i] << '\n';
    }
    file.close();
    return true;
}

/* Автор: Горчак Дмитрий, 212
   Записывает одно двумерное облако точек: в строке две координаты.
   Параметры:
      cloud - описание облака (разбросы по осям и связь координат)
      gauss - датчик случайных чисел
      count - сколько точек построить
   Возвращает true при успешной записи. */
bool save_cloud(const Cloud &cloud, Gauss &gauss, int count) {
    std::ofstream file((std::string(OUT_DIR) + "/" + cloud.name).c_str());

    if (!file.is_open()) {
        return false;
    }
    file << "# x y   (mu = (0, 0), sigma = (" << cloud.sigma_x << ", "
         << cloud.sigma_y << "), rho = " << cloud.rho << ")\n";
    file << std::fixed << std::setprecision(4);
    for (int i = 0; i < count; ++i) {
        float x = 0.0f;
        float y = 0.0f;

        gauss.next_point(0.0f, 0.0f, cloud.sigma_x, cloud.sigma_y, cloud.rho, x, y);
        file << x << ' ' << y << '\n';
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
