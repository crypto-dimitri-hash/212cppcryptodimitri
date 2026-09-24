/* scatter.h -- рассыпание гауссовых точек вокруг окружностей.
   Для каждой фигуры с радиусом (круг или конус) вокруг её центра
   строится облако точек по двумерному нормальному закону. Разброс
   sigma берётся как доля радиуса, и каждая точка сразу получает
   номер своего блока сетки.
   Автор: Горчак Дмитрий, 212 */

#ifndef SCATTER_H
#define SCATTER_H

#include <vector>

#include "figures_io.h"
#include "gauss.h"
#include "grid.h"

/* Рассыпает по count точек вокруг каждой окружности.
   sigma = r * sigma_part, rho -- наклон облака (0 -- без наклона).
   cones_only -- брать только конусы (круги без высоты пропускаются).
   В owner для каждой точки записывается номер фигуры, вокруг которой
   она построена (индекс в figures). */
void scatter_around_circles(const std::vector<FigurePtr> &figures, const Grid &grid,
                            Gauss &gauss, int count, float sigma_part, float rho,
                            bool cones_only, std::vector<GridPoint> &points,
                            std::vector<int> &owner);

#endif
