set terminal png truecolor size 1000,800 font 'Verdana,10'
set grid
set xlabel "x"
set ylabel "y"
load 'out/range.gp'
load 'out/gauss_cone.gp'

set output "out/gauss_points_plan.png"
set title "Gaussian points inside circles: sigma = r / 2, the circle is 2 sigma"
set size ratio -1
set key top left
set style fill transparent solid 0.12 border lc rgb "#1f4e79"
plot "out/gauss_grid.txt" using 1:2 with lines lw 1 lc rgb "#d8d8d8" notitle, \
     "out/gauss_circles.txt" using 1:2:3 with circles lc rgb "#4a90d9" title "circles", \
     "out/gauss_points.txt" using 1:2 with points pt 7 ps 0.5 lc rgb "#1f4e79" title "points", \
     "out/gauss_means.txt" using 1:2 with points pt 2 ps 1.6 lw 2 lc rgb "#333333" title "center", \
     "out/gauss_means.txt" using 3:4 with points pt 6 ps 1.6 lw 2 lc rgb "#b22222" title "sample mean"
unset output

set terminal png truecolor size 1300,560 font 'Verdana,10'
set output "out/gauss_points_cone.png"
set multiplot layout 1,2 title sprintf("The tallest cone: r = %.1f, h = %.1f, sigma = %.2f", r, h, sigma)

set title "Top view: 2000 points, the circle is 2 sigma"
set size ratio -1
unset xrange
unset yrange
set xrange [x0 - 2*r : x0 + 2*r]
set yrange [y0 - 2*r : y0 + 2*r]
set object 1 circle at x0, y0 size r fillstyle empty border lc rgb "#1f4e79" lw 2 front
plot "out/gauss_cone.txt" using 1:2 with points pt 7 ps 0.25 lc rgb "#4a90d9" notitle, \
     "out/gauss_cone_center.txt" using 1:2 with points pt 2 ps 2 lw 2 lc rgb "#333333" title "center", \
     "out/gauss_cone_center.txt" using 3:4 with points pt 6 ps 2 lw 2 lc rgb "#b22222" title "sample mean"
unset object 1

set title "Projection on x: histogram scaled to the cone height"
set size noratio
set xrange [x0 - 2*r : x0 + 2*r]
set yrange [0 : h * 1.15]
set xlabel "x"
set ylabel "height"
set boxwidth (4.0 * r / 24) * 0.9
set style fill solid 0.45 border lc rgb "#1f4e79"
plot "out/gauss_hist.txt" using 1:3 with boxes lc rgb "#4a90d9" title "histogram of x", \
     h * exp(-(x - x0)**2 / (2 * sigma**2)) with lines lw 2 lc rgb "#b22222" title "Gauss curve", \
     (abs(x - x0) < r ? h * (1 - abs(x - x0) / r) : 0) with lines lw 2 dt 2 lc rgb "#333333" title "cone profile"

unset multiplot
unset output

print "plots are ready: out/gauss_points_plan.png, out/gauss_points_cone.png"

set terminal qt size 760,620 font 'Verdana,9'
load 'out/range.gp'
set title "Gaussian points inside circles"
set size ratio -1
set key top left
set xlabel "x"
set ylabel "y"
set style fill transparent solid 0.12 border lc rgb "#1f4e79"
plot "out/gauss_circles.txt" using 1:2:3 with circles lc rgb "#4a90d9" title "circles", \
     "out/gauss_points.txt" using 1:2 with points pt 7 ps 0.6 lc rgb "#1f4e79" title "points", \
     "out/gauss_means.txt" using 3:4 with points pt 6 ps 1.6 lw 2 lc rgb "#b22222" title "sample mean"

pause -1 "window is open, press Enter to close it"
