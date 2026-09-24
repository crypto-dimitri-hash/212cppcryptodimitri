/* scatter.cpp -- реализация рассыпания гауссовых точек.
   Автор: Горчак Дмитрий, 212 */

#include "scatter.h"

#include "cone.h"

/* Автор: Горчак Дмитрий, 212
   Рассыпает точки вокруг всех окружностей из списка фигур. Какая
   фигура перед нами, выясняется через dynamic_cast: у точки радиуса
   нет, круг и конус -- наследники Circle, поэтому оба подходят.
   Параметры:
      figures    - список фигур
      grid       - сетка блоков, по ней каждой точке даётся блок
      gauss      - датчик нормальных чисел
      count      - сколько точек рассыпать вокруг одной окружности
      sigma_part - sigma как доля радиуса
      rho        - коэффициент связи координат
      cones_only - true: только конусы, false: круги и конусы
      points     - сюда добавляются точки
      owner      - сюда добавляется номер фигуры для каждой точки
   Ничего не возвращает. */
void scatter_around_circles(const std::vector<FigurePtr> &figures, const Grid &grid,
                            Gauss &gauss, int count, float sigma_part, float rho,
                            bool cones_only, std::vector<GridPoint> &points,
                            std::vector<int> &owner) {
    for (std::size_t i = 0; i < figures.size(); ++i) {
        const Circle *circle = dynamic_cast<const Circle *>(figures[i].get());

        if (circle == nullptr) {
            continue;                               /* у точки нет радиуса */
        }
        if (cones_only && dynamic_cast<const Cone *>(circle) == nullptr) {
            continue;                               /* круг без высоты */
        }

        float sigma = circle->get_r() * sigma_part;

        for (int k = 0; k < count; ++k) {
            float x = 0.0f;
            float y = 0.0f;

            gauss.next_point(circle->get_x(), circle->get_y(), sigma, sigma, rho, x, y);
            points.push_back(grid.make_point(x, y));
            owner.push_back(static_cast<int>(i));
        }
    }
}
