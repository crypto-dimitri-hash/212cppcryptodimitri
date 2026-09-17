# plot_cones.gp -- visualisation of the cone field (top view and landscape)
# Автор: Горчак Дмитрий, 212
#
# Сначала запустить программу cones_app: она создаёт три файла --
# cones_plan.txt (вид сверху), cones_field.txt (сетка высот)
# и cones_range.gp (границы поля по x и y). Затем из папки cones:
#   gnuplot plot/plot_cones.gp
# Готовые рисунки: cones_plan.png, cones_map.png, cones_landscape.png.
# Чтобы посмотреть картинку в окне, замените терминал на: set terminal qt persist

set terminal pngcairo size 1000,800 font 'Verdana,10'
set grid
set xlabel "x"
set ylabel "y"

# Границы поля посчитаны программой, а не заданы руками
load 'cones_range.gp'

# 1. Вид сверху: круги оснований и их центры
set output "cones_plan.png"
set title "Cones on a plane: bases (top view)"
set size ratio -1
set key top left
set style fill transparent solid 0.25 border lc rgb "#1f4e79"
plot "cones_plan.txt" using 1:2:3 with circles lc rgb "#4a90d9" title "base circles", \
     "cones_plan.txt" using 1:2 with points pt 7 ps 0.8 lc rgb "#b22222" title "centres", \
     "cones_plan.txt" using 1:2:(sprintf("h=%.1f", $4)) with labels \
         offset 0,1.2 font ",8" textcolor rgb "#404040" notitle
unset output

# 2. Карта высот: тот же ландшафт, вид строго сверху
set output "cones_map.png"
set title "Height map z(x, y)"
set view map
set pm3d at b
# та же «земляная» палитра, что и у ландшафта: карта читается как топографическая
set palette defined (0 "#eef3e6", 0.2 "#8fbf5a", 0.5 "#e0c878", 0.8 "#a8743a", 1 "#ffffff")
set cblabel "height"
unset key
splot "cones_field.txt" using 1:2:3 with pm3d notitle
unset output

# 3. Ландшафт: поверхность z(x, y) над плоскостью
set output "cones_landscape.png"
set title "Cone landscape z(x, y)"
unset view
set view 55, 35, 1.0, 1.0
set size ratio 1
set xyplane at 0
set zlabel "h"
# для ландшафта палитра «земляная»: от травы через песок к вершинам
set palette defined (0 "#eef3e6", 0.2 "#8fbf5a", 0.5 "#e0c878", 0.8 "#a8743a", 1 "#ffffff")
set pm3d
set hidden3d
splot "cones_field.txt" using 1:2:3 with pm3d notitle
unset output

print "plots are ready: cones_plan.png, cones_map.png, cones_landscape.png"
