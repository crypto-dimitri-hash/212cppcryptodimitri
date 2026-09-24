/* cluster.cpp -- реализация поиска кластеров.
   Автор: Горчак Дмитрий, 212 */

#include "cluster.h"

#include <algorithm>    /* std::sort */
#include <fstream>      /* std::ofstream */
#include <iomanip>      /* std::fixed, std::setprecision */
#include <queue>        /* std::queue -- очередь для волнового алгоритма */

/* Автор: Горчак Дмитрий, 212
   Строит матрицу попарных расстояний. Матрица симметричная, на
   главной диагонали нули, поэтому считается только половина, а вторая
   заполняется копированием.
   Параметры:
      points - облако точек
   Возвращает матрицу n x n. */
std::vector<std::vector<float> > distance_matrix(const std::vector<GridPoint> &points) {
    std::size_t n = points.size();
    std::vector<std::vector<float> > distances(n, std::vector<float>(n, 0.0f));

    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = i + 1; j < n; ++j) {
            float distance = points[i].distance_to(points[j]);

            distances[i][j] = distance;
            distances[j][i] = distance;
        }
    }
    return distances;
}

/* Автор: Горчак Дмитрий, 212
   Строит двоичную матрицу связей: единица означает, что точки ближе
   порога, то есть между ними есть ребро графа. Точка сама с собой не
   связывается.
   Параметры:
      distances - матрица расстояний
      threshold - пороговое расстояние
   Возвращает матрицу из нулей и единиц. */
std::vector<std::vector<char> > binary_matrix(
    const std::vector<std::vector<float> > &distances, float threshold) {
    std::size_t n = distances.size();
    std::vector<std::vector<char> > links(n, std::vector<char>(n, 0));

    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            if (i != j && distances[i][j] <= threshold) {
                links[i][j] = 1;
            }
        }
    }
    return links;
}

/* Автор: Горчак Дмитрий, 212
   Считает число соседей каждой точки -- число единиц в строке матрицы.
   Параметры:
      links - двоичная матрица связей
   Возвращает вектор с числом соседей для каждой точки. */
std::vector<int> neighbour_counts(const std::vector<std::vector<char> > &links) {
    std::vector<int> counts(links.size(), 0);

    for (std::size_t i = 0; i < links.size(); ++i) {
        for (std::size_t j = 0; j < links.size(); ++j) {
            counts[i] += links[i][j];
        }
    }
    return counts;
}

/* Автор: Горчак Дмитрий, 212
   Волновой алгоритм («лесной пожар»). Все точки сначала в состоянии 0.
   Очередная непройденная точка поджигается -- получает волну 1; её
   соседям с нулём ставится 2, их соседям 3 и так далее. Когда волна
   затухает, кластер найден целиком, а наибольший номер волны
   показывает, за сколько шагов пожар дошёл до самой дальней точки.
   Очередь точек хранится в std::queue.
   Параметры:
      links   - двоичная матрица связей
      cluster - сюда записывается номер кластера каждой точки (с 1)
      wave    - сюда записывается номер волны каждой точки
   Возвращает число найденных кластеров. */
int clusters_wave(const std::vector<std::vector<char> > &links, std::vector<int> &cluster,
                  std::vector<int> &wave) {
    std::size_t n = links.size();
    int number = 0;

    cluster.assign(n, 0);
    wave.assign(n, 0);

    for (std::size_t start = 0; start < n; ++start) {
        if (wave[start] != 0) {
            continue;                       /* волна здесь уже прошла */
        }

        std::queue<std::size_t> fire;

        ++number;
        wave[start] = 1;                    /* поджигаем точку */
        cluster[start] = number;
        fire.push(start);

        while (!fire.empty()) {
            std::size_t current = fire.front();

            fire.pop();
            for (std::size_t next = 0; next < n; ++next) {
                if (links[current][next] == 1 && wave[next] == 0) {
                    wave[next] = wave[current] + 1;
                    cluster[next] = number;
                    fire.push(next);
                }
            }
        }
    }
    return number;
}

/* Автор: Горчак Дмитрий, 212
   Прорубание просеки: от очередной непройденной точки идём по связям
   вглубь, пока есть куда идти, и возвращаемся, когда упёрлись. Вместо
   очереди здесь стек: он хранит точки, к которым ещё нужно вернуться.
   Результат такой же, как у волнового алгоритма, но порядок обхода
   другой -- сначала уходим как можно дальше, а не расходимся кольцами.
   Параметры:
      links   - двоичная матрица связей
      cluster - сюда записывается номер кластера каждой точки (с 1)
   Возвращает число найденных кластеров. */
int clusters_path(const std::vector<std::vector<char> > &links,
                  std::vector<int> &cluster) {
    std::size_t n = links.size();
    int number = 0;

    cluster.assign(n, 0);

    for (std::size_t start = 0; start < n; ++start) {
        if (cluster[start] != 0) {
            continue;
        }

        std::vector<std::size_t> path;      /* стек пройденных точек */

        ++number;
        cluster[start] = number;
        path.push_back(start);

        while (!path.empty()) {
            std::size_t current = path.back();

            path.pop_back();
            for (std::size_t next = 0; next < n; ++next) {
                if (links[current][next] == 1 && cluster[next] == 0) {
                    cluster[next] = number;
                    path.push_back(next);   /* к этой точке вернёмся позже */
                }
            }
        }
    }
    return number;
}

/* Автор: Горчак Дмитрий, 212
   Считает, сколько точек попало в каждый кластер.
   Параметры:
      cluster - номера кластеров точек
      count   - число кластеров
   Возвращает вектор размеров: элемент 0 -- размер первого кластера. */
std::vector<int> cluster_sizes(const std::vector<int> &cluster, int count) {
    std::vector<int> sizes(count, 0);

    for (std::size_t i = 0; i < cluster.size(); ++i) {
        if (cluster[i] >= 1 && cluster[i] <= count) {
            ++sizes[cluster[i] - 1];
        }
    }
    return sizes;
}

/* Автор: Горчак Дмитрий, 212
   Подбор порога по минимальному покрывающему дереву. Дерево строится
   алгоритмом Прима: к уже связанной части каждый раз добавляется
   ближайшая внешняя точка. Длины рёбер дерева сортируются; внутри
   сгустков рёбра короткие, а между сгустками появляется заметный
   скачок длины. Порог берётся посередине самого большого скачка.
   Параметры:
      distances - матрица расстояний
      edges     - сюда записываются длины рёбер дерева по возрастанию
   Возвращает предложенный порог; для менее чем двух точек -- 0. */
float mst_threshold(const std::vector<std::vector<float> > &distances,
                    std::vector<float> &edges) {
    std::size_t n = distances.size();
    std::vector<char> in_tree(n, 0);
    std::vector<float> best(n, 0.0f);
    float gap = 0.0f;
    float threshold = 0.0f;

    edges.clear();
    if (n < 2) {
        return 0.0f;
    }

    /* Прим: начинаем с точки 0 и n-1 раз добавляем ближайшую внешнюю */
    for (std::size_t i = 0; i < n; ++i) {
        best[i] = distances[0][i];
    }
    in_tree[0] = 1;

    for (std::size_t step = 1; step < n; ++step) {
        std::size_t nearest = n;

        for (std::size_t i = 0; i < n; ++i) {
            if (in_tree[i] == 0 && (nearest == n || best[i] < best[nearest])) {
                nearest = i;
            }
        }
        in_tree[nearest] = 1;
        edges.push_back(best[nearest]);

        for (std::size_t i = 0; i < n; ++i) {
            if (in_tree[i] == 0 && distances[nearest][i] < best[i]) {
                best[i] = distances[nearest][i];
            }
        }
    }

    std::sort(edges.begin(), edges.end());

    /* Самый большой скачок между соседними длинами рёбер */
    for (std::size_t i = 1; i < edges.size(); ++i) {
        if (edges[i] - edges[i - 1] > gap) {
            gap = edges[i] - edges[i - 1];
            threshold = (edges[i] + edges[i - 1]) / 2.0f;
        }
    }
    if (threshold == 0.0f) {
        threshold = edges.back();
    }
    return threshold;
}

/* Автор: Горчак Дмитрий, 212
   Сохраняет точки с номерами кластеров: gnuplot раскрашивает точки
   по последнему столбцу.
   Параметры:
      filename - имя файла
      points   - облако точек
      cluster  - номера кластеров
   Возвращает true при успешной записи. */
bool save_clusters(const std::string &filename, const std::vector<GridPoint> &points,
                   const std::vector<int> &cluster) {
    std::ofstream file(filename.c_str());

    if (!file.is_open()) {
        return false;
    }

    file << "# x y block_x block_y cluster\n";
    file << std::fixed << std::setprecision(4);
    for (std::size_t i = 0; i < points.size(); ++i) {
        file << points[i].get_x() << ' ' << points[i].get_y() << ' '
             << points[i].get_block_x() << ' ' << points[i].get_block_y() << ' '
             << cluster[i] << '\n';
    }

    file.close();
    return true;
}
