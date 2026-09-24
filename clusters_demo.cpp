/* clusters_demo.cpp -- поиск сгустков в облаке гауссовых точек.
   Облако строится так же, как в программе gauss_points: вокруг каждого
   конуса точки по двумерному нормальному закону, у каждой точки номер
   блока сетки. По облаку строится матрица расстояний, по ней и порогу --
   двоичная матрица связей, и в полученном графе ищутся связные
   компоненты (кластеры) двумя способами: обходом и волновым алгоритмом.
   Автор: Горчак Дмитрий, 212 */

#include <cstdlib>      /* std::atoi, std::atof */
#include <iomanip>      /* std::setw, std::fixed, std::setprecision */
#include <iostream>
#include <string>
#include <vector>

#include "cluster.h"
#include "figures_io.h"
#include "gauss.h"
#include "grid.h"
#include "scatter.h"

const char *const DEFAULT_DATA_FILE = "figures.txt";
const char *const OUT_DIR = "out";
const int POINTS_PER_CONE = 20;      /* сколько точек рассыпать вокруг конуса */
const int GRID_BLOCKS = 10;          /* блоков сетки по каждой оси */
const unsigned SEED = 212;           /* зерно датчика: опыт повторяем */
const float SIGMA_PART = 0.5f;       /* sigma = radius * SIGMA_PART -> 2 sigma = radius */

void print_points(const std::vector<GridPoint> &points, std::size_t limit);
void print_clusters(const std::string &title, int count, const std::vector<int> &cluster);

/* Автор: Горчак Дмитрий, 212
   Точка входа. Читает фигуры, рассыпает вокруг конусов гауссовы точки,
   приписывает им блоки сетки, ищет кластеры и ближайшую точку.
   Параметры:
      argc - количество аргументов + 1
      argv - argv[1] -- файл с фигурами, argv[2] -- точек на конус,
             argv[3] -- коэффициент связи rho (наклон облака)
   Возвращает 0 при успехе и -1 при ошибке. */
int main(int argc, char *argv[]) {
    std::string data_file = DEFAULT_DATA_FILE;
    int per_cone = POINTS_PER_CONE;
    float rho = 0.0f;
    std::vector<FigurePtr> figures;
    std::vector<GridPoint> points;
    FieldBounds bounds = {0.0f, 0.0f, 0.0f, 0.0f};
    Gauss gauss(SEED);

    if (argc > 4) {
        std::cout << "Usage: " << argv[0] << " [data_file] [points_per_cone] [rho]\n";
        return -1;
    }
    if (argc >= 2) {
        data_file = argv[1];
    }
    if (argc >= 3) {
        per_cone = std::atoi(argv[2]);
    }
    if (argc == 4) {
        rho = static_cast<float>(std::atof(argv[3]));
    }
    if (per_cone < 1) {
        std::cout << "Error: points_per_cone must be >= 1\n";
        return -1;
    }

    std::cout << "\n===============================================================\n";
    std::cout << "Gaussian points, grid blocks and clusters\n";
    std::cout << "===============================================================\n\n";

    std::cout << "--- Reading figures from '" << data_file << "' ---\n";
    if (!load_figures(data_file, figures, std::cout) || figures.empty()) {
        std::cout << "Error: could not read '" << data_file << "'.\n";
        return -1;
    }
    bounds = field_bounds(figures, 8.0f);

    Grid grid(bounds.x_min, bounds.x_max, bounds.y_min, bounds.y_max, GRID_BLOCKS,
              GRID_BLOCKS);

    std::cout << "\n--- Grid of blocks ---\n";
    std::cout << "  field  x: [" << bounds.x_min << "; " << bounds.x_max << "], y: ["
              << bounds.y_min << "; " << bounds.y_max << "]\n";
    std::cout << "  blocks " << grid.get_nx() << " x " << grid.get_ny()
              << ", block size " << grid.cell_width() << " x " << grid.cell_height()
              << '\n';

    /* Вокруг каждого конуса рассыпаем точки по двумерному Гауссу.
       sigma берём равной половине радиуса: край основания -- 2 sigma. */
    std::vector<int> owner;

    scatter_around_circles(figures, grid, gauss, per_cone, SIGMA_PART, rho, true, points,
                           owner);
    std::cout << "\n--- Gaussian points around every cone ---\n";
    std::cout << "  points per cone: " << per_cone << ", rho = " << rho << '\n';
    std::cout << "  points created: " << points.size() << '\n';

    if (points.size() < 2) {
        std::cout << "Error: not enough points, add cones to the data file.\n";
        return -1;
    }

    print_points(points, 8);

    /* Матрица расстояний и порог */
    std::vector<std::vector<float> > distances = distance_matrix(points);
    std::vector<float> mst_edges;
    float threshold = mst_threshold(distances, mst_edges);

    std::cout << "\n--- Threshold from the minimum spanning tree ---\n";
    std::cout << "  tree edges: shortest " << mst_edges.front() << ", longest "
              << mst_edges.back() << '\n';
    std::cout << "  suggested threshold: " << threshold << '\n';

    std::vector<std::vector<char> > links = binary_matrix(distances, threshold);
    std::vector<int> neighbours = neighbour_counts(links);
    int most = 0;

    for (std::size_t i = 0; i < neighbours.size(); ++i) {
        if (neighbours[i] > neighbours[most]) {
            most = static_cast<int>(i);
        }
    }
    std::cout << "\n--- Binary matrix of links ---\n";
    std::cout << "  size " << links.size() << " x " << links.size()
              << ", the busiest point has " << neighbours[most] << " neighbours\n";

    /* Два способа найти связные компоненты */
    std::vector<int> by_wave;
    std::vector<int> wave;
    std::vector<int> by_path;
    int count_wave = clusters_wave(links, by_wave, wave);
    int count_path = clusters_path(links, by_path);
    int deepest = 0;

    for (std::size_t i = 0; i < wave.size(); ++i) {
        if (wave[i] > deepest) {
            deepest = wave[i];
        }
    }

    print_clusters("Wave algorithm (forest fire)", count_wave, by_wave);
    std::cout << "  the fire needed " << deepest << " waves at most\n";
    print_clusters("Path algorithm (cutting a track)", count_path, by_path);
    std::cout << "  both algorithms found "
              << ((count_wave == count_path) ? "the same number of clusters"
                                             : "different results")
              << '\n';

    /* Поиск ближайшей точки: перебор против сетки блоков */
    float ask_x = (bounds.x_min + bounds.x_max) / 2.0f;
    float ask_y = (bounds.y_min + bounds.y_max) / 2.0f;
    long checks_brute = 0;
    long checks_grid = 0;
    int nearest_brute_index = 0;
    int nearest_grid_index = 0;

    grid.fill(points);
    nearest_brute_index = grid.nearest_brute(points, ask_x, ask_y, checks_brute);
    nearest_grid_index = grid.nearest_by_blocks(points, ask_x, ask_y, checks_grid);

    std::cout << "\n--- Nearest point to (" << ask_x << ", " << ask_y << ") ---\n";
    std::cout << "  full search : point " << nearest_brute_index + 1 << ", "
              << checks_brute << " distances\n";
    std::cout << "  by blocks   : point " << nearest_grid_index + 1 << ", "
              << checks_grid << " distances\n";
    std::cout << "  answers are "
              << ((nearest_brute_index == nearest_grid_index) ? "the same" : "different")
              << ", the grid checked " << checks_brute - checks_grid << " points less\n";
    std::cout << "  ";
    points[nearest_grid_index].print(std::cout);
    std::cout << '\n';

    /* Данные для gnuplot */
    std::cout << "\n--- Data for gnuplot (folder '" << OUT_DIR << "') ---\n";
    if (save_clusters(std::string(OUT_DIR) + "/points.txt", points, by_wave)) {
        std::cout << "  points.txt -- x, y, block and cluster of every point\n";
    }
    if (grid.save_lines(std::string(OUT_DIR) + "/grid_lines.txt")) {
        std::cout << "  grid_lines.txt -- lines of the block grid\n";
    }

    std::cout << "\nRun 'gnuplot plot_clusters.gp' to draw the clusters.\n";
    std::cout << "Program finished.\n";
    return 0;
}

/* Автор: Горчак Дмитрий, 212
   Печатает начало таблицы точек: собственные координаты и координаты
   блока сетки.
   Параметры:
      points - облако точек
      limit  - сколько строк напечатать
   Ничего не возвращает. */
void print_points(const std::vector<GridPoint> &points, std::size_t limit) {
    std::cout << "\n--- First points (x, y and the block they fell into) ---\n";
    std::cout << std::setw(5) << "N" << std::setw(10) << "x" << std::setw(10) << "y"
              << std::setw(10) << "block x" << std::setw(10) << "block y" << '\n';
    std::cout << std::string(45, '-') << '\n';
    std::cout << std::fixed << std::setprecision(2);
    for (std::size_t i = 0; i < points.size() && i < limit; ++i) {
        std::cout << std::setw(5) << i + 1 << std::setw(10) << points[i].get_x()
                  << std::setw(10) << points[i].get_y() << std::setw(10)
                  << points[i].get_block_x() << std::setw(10)
                  << points[i].get_block_y() << '\n';
    }
    std::cout << "  ... " << points.size() << " points in total\n";
}

/* Автор: Горчак Дмитрий, 212
   Печатает результат кластеризации: число кластеров и размеры первых
   из них.
   Параметры:
      title   - название способа
      count   - число кластеров
      cluster - номера кластеров точек
   Ничего не возвращает. */
void print_clusters(const std::string &title, int count, const std::vector<int> &cluster) {
    std::vector<int> sizes = cluster_sizes(cluster, count);

    std::cout << "\n--- " << title << " ---\n";
    std::cout << "  clusters found: " << count << '\n';
    std::cout << "  sizes:";
    for (std::size_t i = 0; i < sizes.size() && i < 12; ++i) {
        std::cout << ' ' << sizes[i];
    }
    if (sizes.size() > 12) {
        std::cout << " ...";
    }
    std::cout << '\n';
}
