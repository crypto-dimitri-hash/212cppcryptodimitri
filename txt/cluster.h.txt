/* cluster.h -- поиск сгустков точек (кластеров).
   Порядок работы такой: по облаку точек строится матрица расстояний,
   по ней и порогу -- двоичная матрица связей (граф), а связная
   компонента этого графа и есть кластер. Компоненты ищутся двумя
   способами: обходом («прорубание просеки») и волновым алгоритмом
   («лесной пожар»).
   Автор: Горчак Дмитрий, 212 */

#ifndef CLUSTER_H
#define CLUSTER_H

#include <string>
#include <vector>

#include "grid_point.h"

/* Матрица расстояний n x n: элемент [i][j] -- расстояние между точками */
std::vector<std::vector<float> > distance_matrix(const std::vector<GridPoint> &points);

/* Двоичная матрица связей: 1, если расстояние не больше порога */
std::vector<std::vector<char> > binary_matrix(
    const std::vector<std::vector<float> > &distances, float threshold);

/* Число соседей каждой точки -- число единиц в её строке */
std::vector<int> neighbour_counts(const std::vector<std::vector<char> > &links);

/* Волновой алгоритм: поджигаем точку, волна расходится по связям.
   cluster[i] -- номер кластера точки, wave[i] -- номер волны, на которой
   точка загорелась (мощность кластера равна наибольшему номеру волны).
   Возвращает число найденных кластеров. */
int clusters_wave(const std::vector<std::vector<char> > &links, std::vector<int> &cluster,
                  std::vector<int> &wave);

/* Прорубание просеки: обход в глубину от очередной непройденной точки.
   cluster[i] -- номер кластера точки. Возвращает число кластеров. */
int clusters_path(const std::vector<std::vector<char> > &links,
                  std::vector<int> &cluster);

/* Размеры кластеров: сколько точек в каждом (по номерам с 1) */
std::vector<int> cluster_sizes(const std::vector<int> &cluster, int count);

/* Порог по минимальному покрывающему дереву: строится дерево, его рёбра
   сортируются и ищется самый большой скачок длины. Порог берётся между
   короткими рёбрами (внутри сгустков) и длинными (между сгустками).
   В edges записываются длины рёбер дерева по возрастанию. */
float mst_threshold(const std::vector<std::vector<float> > &distances,
                    std::vector<float> &edges);

/* Сохранение точек с номерами кластеров для gnuplot: x y cluster */
bool save_clusters(const std::string &filename, const std::vector<GridPoint> &points,
                   const std::vector<int> &cluster);

#endif
