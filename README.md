# cones — фигуры на плоскости: Point → Circle → Cone

Автор: Горчак Дмитрий, 212

Репозиторий: <https://github.com/crypto-dimitri-hash/212cppcryptodimitri>

В терминале MSYS2 папка проекта — `~/cones`.

## Программы

| Программа | Файл | Задача |
|---|---|---|
| `figures` | `main.cpp` | иерархия Point → Circle → Cone, объекты из файла, объём по запросу |
| `figures_list` | `list_demo.cpp` | двунаправленный список `std::list`, порядок по площади и по объёму |
| `gauss_points` | `gauss_points.cpp` | точки по Гауссу внутри окружностей, блоки сетки, среднее μ, гистограмма |
| `gauss_demo` | `gauss_demo.cpp` | проверка датчика: одномерный и двумерный Гаусс |
| `clusters` | `clusters_demo.cpp` | матрица расстояний, порог, кластеры волной и просекой |

Сборка всех программ и запуск:

```bash
cd ~/cones
bash build.sh
./bin/figures
gnuplot plot_figures.gp
```

У каждой программы свой скрипт gnuplot: `plot_figures.gp`, `plot_gauss_points.gp`,
`plot_gauss.gp`, `plot_clusters.gp`. Скрипт сохраняет рисунки в `out/` и открывает
окно с одним из них; окно закрывается клавишей Enter в терминале.

## Сколько чисел в строке — такая и фигура

| Числа | Что создаётся | Вычисляется методами |
|---|---|---|
| `x y` | `Point` — точка | — |
| `x y r` | `Circle : Point` — круг | `area()` |
| `x y r h` | `Cone : Circle` — конус | `area()`, `volume()` |

Поля закрыты (`private`), доступ через `get`/`set`. Объём печатается по запросу:
`./bin/figures figures.txt 9` — номер фигуры из таблицы.

## Классы и модули

| Файлы | Что внутри |
|---|---|
| `point.h/.cpp`, `circle.h/.cpp`, `cone.h/.cpp` | иерархия фигур |
| `figures_io.h/.cpp` | чтение файла, таблица, данные для gnuplot |
| `gauss.h/.cpp` | датчик нормальных чисел |
| `grid_point.h/.cpp`, `grid.h/.cpp` | точка с номером блока, сетка блоков |
| `scatter.h/.cpp` | точки по Гауссу вокруг окружностей |
| `cluster.h/.cpp` | матрица расстояний, порог, связные компоненты |

`txt/` — копии всех исходников в виде `.txt` (`point.txt`, `point-h.txt`),
обновляются командой `bash make_txt.sh`. `bin/` и `out/` в репозиторий не попадают.
