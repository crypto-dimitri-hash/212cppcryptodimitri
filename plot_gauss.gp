# plot_gauss.gp -- illustrations for the Gaussian distribution
# Автор: Горчак Дмитрий, 212
#
# Сначала запустить программу gauss_demo: она создаёт в папке out файлы
# gauss1d.txt, gauss2d_rho00.txt и gauss2d_rho07.txt.
# Затем из папки cones:
#   gnuplot plot_gauss.gp
# Готовые рисунки: out/gauss1d.png и out/gauss2d.png.

set terminal pngcairo size 1000,700 font 'Verdana,10'

# 1. Одномерный случай: гистограмма выборки и теоретическая кривая
set output "out/gauss1d.png"
set title "One-dimensional Gaussian: sample histogram and theoretical curve"
set xlabel "value"
set ylabel "share of the sample"
set grid
set xrange [-4:4]
set key top right
set boxwidth 0.2
set style fill transparent solid 0.35 border lc rgb "#1f4e79"

bin = 0.2                      # ширина столбца гистограммы
n = 5000.0                     # размер выборки
mu = 0.0
sigma = 1.0
density(x) = exp(-(x - mu)**2 / (2 * sigma**2)) / (sigma * sqrt(2 * pi))

plot "out/gauss1d.txt" using (bin * floor($1 / bin) + bin / 2):(1.0 / (n * bin)) \
         smooth freq with boxes lc rgb "#4a90d9" title "histogram", \
     density(x) with lines lw 2 lc rgb "#b22222" title "density N(0, 1)"
unset output

# 2. Двумерный случай: облако точек без связи координат и со связью
set output "out/gauss2d.png"
set multiplot layout 1,2 title "Two-dimensional Gaussian: rho = 0 and rho = 0.7"
set size ratio -1
set xrange [-4:4]
set yrange [-4:4]
set xlabel "x"
set ylabel "y"
unset key

set title "rho = 0: axes of the cloud follow the axes"
plot "out/gauss2d_rho00.txt" using 1:2 with points pt 7 ps 0.3 lc rgb "#1f4e79" notitle

set title "rho = 0.7: the cloud is tilted"
plot "out/gauss2d_rho07.txt" using 1:2 with points pt 7 ps 0.3 lc rgb "#b22222" notitle

unset multiplot
unset output

print "plots are ready: out/gauss1d.png, out/gauss2d.png"
