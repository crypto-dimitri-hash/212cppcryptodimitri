/* gauss.cpp -- реализация датчика нормально распределённых чисел.
   Автор: Горчак Дмитрий, 212 */

#include "gauss.h"

#include <cmath>        /* std::sqrt */
#include <stdexcept>    /* std::invalid_argument */

/* Автор: Горчак Дмитрий, 212
   Конструктор. Зерно определяет всю последовательность чисел:
   с одинаковым зерном программа выдаёт одинаковые точки.
   Параметры:
      seed - зерно датчика
   Ничего не возвращает. */
Gauss::Gauss(unsigned seed) : engine_(seed), normal_(0.0f, 1.0f) {
}

/* Автор: Горчак Дмитрий, 212
   Число из стандартного нормального распределения: среднее 0,
   стандартное отклонение 1.
   Параметров нет.
   Возвращает случайное число. */
float Gauss::next_standard() {
    return normal_(engine_);
}

/* Автор: Горчак Дмитрий, 212
   Число из нормального распределения с заданными параметрами.
   Сдвиг на mu переносит центр, умножение на sigma меняет разброс.
   Параметры:
      mu    - среднее значение (центр распределения)
      sigma - стандартное отклонение (разброс), sigma >= 0
   Возвращает случайное число; при sigma < 0 бросает std::invalid_argument. */
float Gauss::next_value(float mu, float sigma) {
    if (sigma < 0.0f) {
        throw std::invalid_argument("sigma must be >= 0");
    }
    return mu + sigma * next_standard();
}

/* Автор: Горчак Дмитрий, 212
   Точка двумерного нормального распределения. Берутся два независимых
   числа z1 и z2 из N(0, 1), после чего второе «подмешивается» к первому:
      x = mu_x + sigma_x * z1
      y = mu_y + sigma_y * (rho * z1 + sqrt(1 - rho^2) * z2)
   При rho = 0 координаты независимы и облако точек вытянуто вдоль осей;
   при rho, отличном от нуля, облако наклонено.
   Параметры:
      mu_x, mu_y       - центр распределения
      sigma_x, sigma_y - разброс по каждой оси
      rho              - коэффициент связи координат, -1 < rho < 1
      x, y             - сюда записывается результат
   Ничего не возвращает; при недопустимом rho бросает std::invalid_argument. */
void Gauss::next_point(float mu_x, float mu_y, float sigma_x, float sigma_y, float rho,
                       float &x, float &y) {
    float z1 = 0.0f;
    float z2 = 0.0f;

    if (rho <= -1.0f || rho >= 1.0f) {
        throw std::invalid_argument("rho must be between -1 and 1");
    }
    z1 = next_standard();
    z2 = next_standard();

    x = mu_x + sigma_x * z1;
    y = mu_y + sigma_y * (rho * z1 + std::sqrt(1.0f - rho * rho) * z2);
}
