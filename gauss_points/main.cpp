/* gauss_points.cpp -- точки по Гауссу внутри окружностей на плоскости.
   Вокруг центра каждой окружности (круга или основания конуса)
   рассыпаются точки по двумерному нормальному закону с sigma = r / 2:
   тогда край окружности лежит на расстоянии двух сигм от центра.
   У каждой точки две собственные координаты и две координаты блока
   сетки. По каждому облаку считается выборочное среднее
   mu = (1/n) * sum x_i и доля точек внутри окружности.
   Для самого высокого конуса строится большая выборка и гистограмма её
   проекции на ось x: высота гистограммы приводится к высоте конуса, и
   видно, что наибольшее скопление точек -- в центре, как вершина конуса.
   Это заготовка облака точек для задачи о кластерах.
   Автор: Горчак Дмитрий, 212 */

#include <cmath>        /* std::sqrt, std::fabs, std::floor */
#include <cstdlib>      /* std::atoi */
#include <fstream>      /* std::ofstream */
#include <iomanip>      /* std::setw, std::fixed, std::setprecision */
#include <iostream>
#include <filesystem>     /* std::filesystem::create_directories */
#include <string>
#include <vector>

#include "cone.h"
#include "figures_io.h"
#include "gauss.h"
#include "grid.h"
#include "scatter.h"

const char *const DEFAULT_DATA_FILE = "figures.txt";
const char *const OUT_DIR = "out";
const int POINTS_PER_CIRCLE = 20;   /* как в примере с семинара */
const int BIG_SAMPLE = 2000;        /* выборка для гистограммы одного конуса */
const int HIST_BINS = 24;           /* столбцов гистограммы на отрезке 4r */
const int GRID_BLOCKS = 10;         /* блоков сетки по каждой оси */
const unsigned SEED = 212;          /* зерно датчика: опыт повторяем */
const float SIGMA_PART = 0.5f;      /* sigma = r / 2, край круга -- 2 sigma */
const float MARGIN_PERCENT = 8.0f;  /* запас поля вокруг фигур */
const double THEORY_CIRCLE = 86.5;  /* % точек внутри круга радиуса 2 sigma */
const double THEORY_AXIS = 95.4;    /* % проекций внутри x0 +- 2 sigma */

/* Итоги по одному облаку точек */
struct CloudStats {
    double mean_x;          /* mu_x = (1/n) * sum x_i */
    double mean_y;          /* mu_y = (1/n) * sum y_i */
    double deviation_x;     /* выборочное отклонение по x */
    double inside_circle;   /* доля точек внутри окружности, % */
    double inside_axis;     /* доля точек с |x - x0| <= 2 sigma, % */
};

CloudStats cloud_stats(const std::vector<GridPoint> &points, std::size_t from,
                       std::size_t to, const Circle &circle, float sigma);
int tallest_cone(const std::vector<FigurePtr> &figures);
void print_first_points(const std::vector<GridPoint> &points, std::size_t limit);
bool save_scatter(const std::vector<FigurePtr> &figures,
                  const std::vector<GridPoint> &points, const std::vector<int> &owner,
                  const std::vector<CloudStats> &stats);
int save_histogram(const std::string &filename, const std::vector<GridPoint> &points,
                   const Cone &cone, float &peak_x);
bool save_cone(const std::vector<GridPoint> &points, const Cone &cone, float sigma,
               const CloudStats &stats);

/* Автор: Горчак Дмитрий, 212
   Точка входа. Читает фигуры, рассыпает точки вокруг окружностей,
   печатает средние и доли попаданий, строит гистограмму для одного
   конуса и сохраняет всё для gnuplot.
   Параметры:
      argc - количество аргументов + 1
      argv - argv[1] -- файл с фигурами, argv[2] -- точек на окружность
   Возвращает 0 при успехе и -1 при ошибке. */
int main(int argc, char *argv[]) {
    std::filesystem::create_directories(OUT_DIR);   /* папка для результатов */
    std::string data_file = DEFAULT_DATA_FILE;
    int per_circle = POINTS_PER_CIRCLE;
    std::vector<FigurePtr> figures;
    std::vector<GridPoint> points;
    std::vector<int> owner;
    std::vector<CloudStats> stats;
    Gauss gauss(SEED);

    if (argc > 3) {
        std::cout << "Usage: ./gauss_points [data_file] [points_per_circle]\n";
        return -1;
    }
    if (argc >= 2) {
        data_file = argv[1];
    }
    if (argc == 3) {
        per_circle = std::atoi(argv[2]);
    }
    if (per_circle < 2) {
        std::cout << "Error: points_per_circle must be >= 2\n";
        return -1;
    }

    std::cout << "\n===============================================================\n";
    std::cout << "Gaussian points inside circles on a plane\n";
    std::cout << "===============================================================\n\n";

    std::cout << "--- Reading figures from '" << data_file << "' ---\n";
    if (!load_figures(data_file, figures, std::cout) || figures.empty()) {
        std::cout << "Error: could not read '" << data_file << "'.\n";
        return -1;
    }

    FieldBounds bounds = field_bounds(figures, MARGIN_PERCENT);
    Grid grid(bounds.x_min, bounds.x_max, bounds.y_min, bounds.y_max, GRID_BLOCKS,
              GRID_BLOCKS);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n--- Grid of blocks ---\n";
    std::cout << "  " << grid.get_nx() << " x " << grid.get_ny() << " blocks, block size "
              << grid.cell_width() << " x " << grid.cell_height() << '\n';

    /* Облака точек вокруг всех окружностей: и кругов, и конусов */
    scatter_around_circles(figures, grid, gauss, per_circle, SIGMA_PART, 0.0f, false,
                           points, owner);
    std::cout << "\n--- Points around every circle ---\n";
    std::cout << "  sigma = r / 2, so the edge of a circle is 2 sigma away from its center\n";
    std::cout << "  points per circle: " << per_circle << ", points created: "
              << points.size() << '\n';
    if (points.empty()) {
        std::cout << "Error: there are no circles or cones in the data file.\n";
        return -1;
    }

    print_first_points(points, 6);

    /* Выборочное среднее каждого облака: mu = (1/n) * sum x_i */
    std::cout << "\n--- Sample mean of every cloud: mu = (1/n) * sum x_i ---\n";
    std::cout << std::setw(4) << "N" << std::setw(8) << "type" << std::setw(9) << "x0"
              << std::setw(8) << "y0" << std::setw(7) << "r" << std::setw(9) << "mu_x"
              << std::setw(8) << "mu_y" << std::setw(8) << "shift" << std::setw(10)
              << "inside" << '\n';
    std::cout << std::string(71, '-') << '\n';

    double total_circle = 0.0;
    double total_axis = 0.0;
    std::size_t from = 0;

    while (from < points.size()) {
        std::size_t to = from;

        while (to < points.size() && owner[to] == owner[from]) {
            ++to;
        }

        const Circle &circle = dynamic_cast<const Circle &>(*figures[owner[from]]);
        float sigma = circle.get_r() * SIGMA_PART;
        CloudStats one = cloud_stats(points, from, to, circle, sigma);
        double shift = std::sqrt((one.mean_x - circle.get_x()) * (one.mean_x - circle.get_x()) +
                                 (one.mean_y - circle.get_y()) * (one.mean_y - circle.get_y()));

        stats.push_back(one);
        total_circle += one.inside_circle * (to - from);
        total_axis += one.inside_axis * (to - from);

        std::cout << std::setw(4) << owner[from] + 1 << std::setw(8) << circle.type_name()
                  << std::setw(9) << circle.get_x() << std::setw(8) << circle.get_y()
                  << std::setw(7) << circle.get_r() << std::setw(9) << one.mean_x
                  << std::setw(8) << one.mean_y << std::setw(8) << shift << std::setw(8)
                  << std::setprecision(0) << one.inside_circle << " %" << '\n'
                  << std::setprecision(2);
        from = to;
    }
    std::cout << std::string(71, '-') << '\n';
    std::cout << std::setprecision(1);
    std::cout << "  all clouds: inside the circle " << total_circle / points.size()
              << " % (theory " << THEORY_CIRCLE << " %)\n";
    std::cout << "              |x - x0| <= 2 sigma " << total_axis / points.size()
              << " % (theory " << THEORY_AXIS << " %)\n";
    std::cout << std::setprecision(2);

    /* Один конус подробно: большая выборка и гистограмма проекции на x */
    int index = tallest_cone(figures);

    if (index < 0) {
        std::cout << "\nThere are no cones, the histogram is skipped.\n";
    } else {
        const Cone &cone = dynamic_cast<const Cone &>(*figures[index]);
        float sigma = cone.get_r() * SIGMA_PART;
        std::vector<GridPoint> sample;
        std::vector<int> sample_owner;
        std::vector<FigurePtr> only_this(1, figures[index]);

        scatter_around_circles(only_this, grid, gauss, BIG_SAMPLE, SIGMA_PART, 0.0f, true,
                               sample, sample_owner);

        CloudStats big = cloud_stats(sample, 0, sample.size(), cone, sigma);
        float peak_x = 0.0f;
        int peak_count = save_histogram(std::string(OUT_DIR) + "/gauss_hist.txt", sample,
                                        cone, peak_x);

        std::cout << "\n--- The tallest cone in detail: " << BIG_SAMPLE << " points ---\n";
        std::cout << "  cone N " << index + 1 << ": center (" << cone.get_x() << ", "
                  << cone.get_y() << "), r = " << cone.get_r() << ", h = " << cone.get_h()
                  << ", sigma = " << sigma << '\n';
        std::cout << "  sample mean       : (" << big.mean_x << ", " << big.mean_y << ")\n";
        std::cout << "  sample deviation x: " << big.deviation_x << " (sigma " << sigma
                  << ")\n";
        std::cout << std::setprecision(1);
        std::cout << "  inside the circle : " << big.inside_circle << " % (theory "
                  << THEORY_CIRCLE << " %)\n";
        std::cout << std::setprecision(2);
        std::cout << "  histogram of x    : " << HIST_BINS << " bins, the tallest one ("
                  << peak_count << " points) is at x = " << peak_x << ", the center is x0 = "
                  << cone.get_x() << '\n';
        std::cout << "  the histogram is scaled so that its tallest bin equals h = "
                  << cone.get_h() << '\n';

        if (!save_cone(sample, cone, sigma, big)) {
            std::cout << "Error: could not write the files of the cone.\n";
            return -1;
        }
    }

    std::cout << "\n--- Data for gnuplot (folder '" << OUT_DIR << "') ---\n";
    if (!save_scatter(figures, points, owner, stats) ||
        !save_range(std::string(OUT_DIR) + "/range.gp", figures, bounds) ||
        !grid.save_lines(std::string(OUT_DIR) + "/gauss_grid.txt")) {
        std::cout << "Error: could not write the data files.\n";
        return -1;
    }
    std::cout << "  gauss_points.txt  -- x, y, block x, block y, figure of every point\n";
    std::cout << "  gauss_circles.txt -- circles, gauss_means.txt -- centers and means\n";
    std::cout << "  gauss_cone.txt, gauss_hist.txt, gauss_cone.gp -- the tallest cone\n";

    std::cout << "\nRun 'gnuplot plot_gauss_points.gp' to draw the pictures.\n";
    std::cout << "Program finished.\n";
    return 0;
}

/* Автор: Горчак Дмитрий, 212
   Считает по части облака [from, to) выборочные средние, отклонение
   по x и доли попаданий в окружность и в полосу x0 +- 2 sigma.
   Параметры:
      points - облако точек
      from   - номер первой точки облака
      to     - номер точки сразу за последней
      circle - окружность, вокруг которой построено облако
      sigma  - заданный разброс
   Возвращает итоги по облаку. */
CloudStats cloud_stats(const std::vector<GridPoint> &points, std::size_t from,
                       std::size_t to, const Circle &circle, float sigma) {
    CloudStats result = {0.0, 0.0, 0.0, 0.0, 0.0};
    double n = static_cast<double>(to - from);
    int inside_circle = 0;
    int inside_axis = 0;

    for (std::size_t i = from; i < to; ++i) {
        result.mean_x += points[i].get_x();
        result.mean_y += points[i].get_y();
    }
    result.mean_x /= n;
    result.mean_y /= n;

    for (std::size_t i = from; i < to; ++i) {
        double dx = points[i].get_x() - result.mean_x;

        result.deviation_x += dx * dx;
        if (points[i].distance_to(circle) <= circle.get_r()) {
            ++inside_circle;
        }
        if (std::fabs(points[i].get_x() - circle.get_x()) <= 2.0 * sigma) {
            ++inside_axis;
        }
    }
    result.deviation_x = std::sqrt(result.deviation_x / n);
    result.inside_circle = 100.0 * inside_circle / n;
    result.inside_axis = 100.0 * inside_axis / n;
    return result;
}

/* Автор: Горчак Дмитрий, 212
   Находит самый высокий конус.
   Параметры:
      figures - список фигур
   Возвращает его номер в списке или -1, если конусов нет. */
int tallest_cone(const std::vector<FigurePtr> &figures) {
    int best = -1;

    for (std::size_t i = 0; i < figures.size(); ++i) {
        const Cone *cone = dynamic_cast<const Cone *>(figures[i].get());

        if (cone != nullptr &&
            (best < 0 ||
             cone->get_h() > dynamic_cast<const Cone &>(*figures[best]).get_h())) {
            best = static_cast<int>(i);
        }
    }
    return best;
}

/* Автор: Горчак Дмитрий, 212
   Печатает первые точки: собственные координаты и координаты блока.
   Параметры:
      points - облако точек
      limit  - сколько строк напечатать
   Ничего не возвращает. */
void print_first_points(const std::vector<GridPoint> &points, std::size_t limit) {
    std::cout << "\n--- First points: own coordinates and block coordinates ---\n";
    std::cout << std::setw(5) << "N" << std::setw(10) << "x" << std::setw(10) << "y"
              << std::setw(10) << "block x" << std::setw(10) << "block y" << '\n';
    std::cout << std::string(45, '-') << '\n';
    for (std::size_t i = 0; i < points.size() && i < limit; ++i) {
        std::cout << std::setw(5) << i + 1 << std::setw(10) << points[i].get_x()
                  << std::setw(10) << points[i].get_y() << std::setw(10)
                  << points[i].get_block_x() << std::setw(10) << points[i].get_block_y()
                  << '\n';
    }
    std::cout << "  ... " << points.size() << " points in total\n";
}

/* Автор: Горчак Дмитрий, 212
   Сохраняет облако точек, окружности и средние для gnuplot.
   Параметры:
      figures - список фигур
      points  - облако точек
      owner   - номер фигуры для каждой точки
      stats   - итоги по каждому облаку в порядке следования облаков
   Возвращает true при успешной записи. */
bool save_scatter(const std::vector<FigurePtr> &figures,
                  const std::vector<GridPoint> &points, const std::vector<int> &owner,
                  const std::vector<CloudStats> &stats) {
    std::string dir = OUT_DIR;
    std::ofstream out_points((dir + "/gauss_points.txt").c_str());
    std::ofstream out_circles((dir + "/gauss_circles.txt").c_str());
    std::ofstream out_means((dir + "/gauss_means.txt").c_str());

    if (!out_points.is_open() || !out_circles.is_open() || !out_means.is_open()) {
        return false;
    }

    out_points << "# x y block_x block_y figure\n" << std::fixed << std::setprecision(4);
    for (std::size_t i = 0; i < points.size(); ++i) {
        out_points << points[i].get_x() << ' ' << points[i].get_y() << ' '
                   << points[i].get_block_x() << ' ' << points[i].get_block_y() << ' '
                   << owner[i] + 1 << '\n';
    }

    out_circles << "# x y r\n" << std::fixed << std::setprecision(4);
    out_means << "# x0 y0 mu_x mu_y\n" << std::fixed << std::setprecision(4);

    std::size_t cloud = 0;

    for (std::size_t i = 0; i < figures.size(); ++i) {
        const Circle *circle = dynamic_cast<const Circle *>(figures[i].get());

        if (circle == nullptr) {
            continue;
        }
        out_circles << circle->get_x() << ' ' << circle->get_y() << ' ' << circle->get_r()
                    << '\n';
        if (cloud < stats.size()) {
            out_means << circle->get_x() << ' ' << circle->get_y() << ' '
                      << stats[cloud].mean_x << ' ' << stats[cloud].mean_y << '\n';
            ++cloud;
        }
    }
    return true;
}

/* Автор: Горчак Дмитрий, 212
   Строит гистограмму проекции точек на ось x на отрезке x0 +- 2r
   (это x0 +- 4 sigma) и приводит высоту столбцов к высоте конуса:
   самый высокий столбец получает высоту h.
   Параметры:
      filename - имя файла
      points   - выборка точек вокруг конуса
      cone     - конус
      peak_x   - сюда записывается середина самого высокого столбца
   Возвращает число точек в самом высоком столбце, 0 при ошибке. */
int save_histogram(const std::string &filename, const std::vector<GridPoint> &points,
                   const Cone &cone, float &peak_x) {
    std::vector<int> counts(HIST_BINS, 0);
    float left = cone.get_x() - 2.0f * cone.get_r();
    float width = 4.0f * cone.get_r() / HIST_BINS;
    int peak = 0;

    for (std::size_t i = 0; i < points.size(); ++i) {
        int bin = static_cast<int>(std::floor((points[i].get_x() - left) / width));

        if (bin >= 0 && bin < HIST_BINS) {
            ++counts[bin];
        }
    }
    for (int bin = 0; bin < HIST_BINS; ++bin) {
        if (counts[bin] > counts[peak]) {
            peak = bin;
        }
    }
    peak_x = left + (peak + 0.5f) * width;

    std::ofstream file(filename.c_str());

    if (!file.is_open() || counts[peak] == 0) {
        return 0;
    }
    file << "# bin_center count height_scaled_to_h\n" << std::fixed << std::setprecision(4);
    for (int bin = 0; bin < HIST_BINS; ++bin) {
        file << left + (bin + 0.5f) * width << ' ' << counts[bin] << ' '
             << cone.get_h() * counts[bin] / counts[peak] << '\n';
    }
    return counts[peak];
}

/* Автор: Горчак Дмитрий, 212
   Сохраняет большую выборку вокруг конуса и параметры конуса в виде
   команд gnuplot, чтобы в скрипте не было чисел, заданных руками.
   Параметры:
      points - выборка точек вокруг конуса
      cone   - конус
      sigma  - разброс
      stats  - итоги по выборке
   Возвращает true при успешной записи. */
bool save_cone(const std::vector<GridPoint> &points, const Cone &cone, float sigma,
               const CloudStats &stats) {
    std::string dir = OUT_DIR;
    std::ofstream out_points((dir + "/gauss_cone.txt").c_str());
    std::ofstream out_center((dir + "/gauss_cone_center.txt").c_str());
    std::ofstream out_params((dir + "/gauss_cone.gp").c_str());

    if (!out_points.is_open() || !out_center.is_open() || !out_params.is_open()) {
        return false;
    }

    out_center << "# x0 y0 mu_x mu_y\n" << std::fixed << std::setprecision(4);
    out_center << cone.get_x() << ' ' << cone.get_y() << ' ' << stats.mean_x << ' '
               << stats.mean_y << '\n';

    out_points << "# x y\n" << std::fixed << std::setprecision(4);
    for (std::size_t i = 0; i < points.size(); ++i) {
        out_points << points[i].get_x() << ' ' << points[i].get_y() << '\n';
    }

    out_params << std::fixed << std::setprecision(4);
    out_params << "x0 = " << cone.get_x() << '\n';
    out_params << "y0 = " << cone.get_y() << '\n';
    out_params << "r = " << cone.get_r() << '\n';
    out_params << "h = " << cone.get_h() << '\n';
    out_params << "sigma = " << sigma << '\n';
    out_params << "mu_x = " << stats.mean_x << '\n';
    out_params << "mu_y = " << stats.mean_y << '\n';
    return true;
}
