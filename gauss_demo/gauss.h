/* gauss.h -- датчик нормально распределённых (гауссовых) чисел.
   Одномерный случай задаётся средним mu и стандартным отклонением sigma,
   двумерный -- двумя средними, двумя сигмами и коэффициентом связи rho.
   Автор: Горчак Дмитрий, 212 */

#ifndef GAUSS_H
#define GAUSS_H

#include <random>     /* std::mt19937, std::normal_distribution */

/* Генератор случайных чисел. Внутри лежит «вихрь Мерсенна» (mt19937) --
   стандартный датчик псевдослучайных чисел C++, и преобразование
   равномерных чисел в нормальные. Одно и то же зерно (seed) даёт
   одну и ту же последовательность, поэтому опыт можно повторить. */
class Gauss {
public:
    explicit Gauss(unsigned seed);

    /* Число из стандартного нормального распределения N(0, 1) */
    float next_standard();

    /* Число из распределения N(mu, sigma) */
    float next_value(float mu, float sigma);

    /* Точка двумерного нормального распределения.
       Параметр rho (от -1 до 1) задаёт наклон облака точек:
       при rho = 0 оси облака параллельны осям координат. */
    void next_point(float mu_x, float mu_y, float sigma_x, float sigma_y, float rho,
                    float &x, float &y);

private:
    std::mt19937 engine_;                        /* источник случайных чисел */
    std::normal_distribution<float> normal_;     /* превращает их в нормальные */
};

#endif
