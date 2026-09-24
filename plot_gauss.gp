set terminal png truecolor size 1000,700 font 'Verdana,10'

set output "out/gauss1d.png"
set title "One-dimensional Gaussian: sample histogram and theoretical curve"
set xlabel "value"
set ylabel "share of the sample"
set grid
set xrange [-4:4]
set key top right
set boxwidth 0.2
set style fill transparent solid 0.35 border lc rgb "#1f4e79"

bin = 0.2
n = 5000.0
mu = 0.0
sigma = 1.0
density(x) = exp(-(x - mu)**2 / (2 * sigma**2)) / (sigma * sqrt(2 * pi))

plot "out/gauss1d.txt" using (bin * floor($1 / bin) + bin / 2):(1.0 / (n * bin)) \
         smooth freq with boxes lc rgb "#4a90d9" title "histogram", \
     density(x) with lines lw 2 lc rgb "#b22222" title "density N(0, 1)"
unset output

set terminal png truecolor size 1200,440 font 'Verdana,10'
set output "out/gauss2d_sigma.png"
set multiplot layout 1,3 title "Two-dimensional Gaussian, rho = 0: the shape is set by sigma"
set size ratio -1
set xrange [-4:4]
set yrange [-4:4]
set xlabel "x"
set ylabel "y"
unset key

set title "sigma_x = sigma_y = 1: round cloud"
plot "out/gauss2d_round.txt" using 1:2 with points pt 7 ps 0.3 lc rgb "#1f4e79" notitle

set title "sigma_x = 2, sigma_y = 0.6: along the x axis"
plot "out/gauss2d_wide.txt" using 1:2 with points pt 7 ps 0.3 lc rgb "#2e7d32" notitle

set title "sigma_x = 0.6, sigma_y = 2: along the y axis"
plot "out/gauss2d_tall.txt" using 1:2 with points pt 7 ps 0.3 lc rgb "#6a3d9a" notitle

unset multiplot
unset output

set output "out/gauss2d_rho.png"
set multiplot layout 1,3 title "Two-dimensional Gaussian, sigma_x = sigma_y = 1: the shape is set by rho"

set title "rho = 0.7: tilted to the right"
plot "out/gauss2d_rho07.txt" using 1:2 with points pt 7 ps 0.3 lc rgb "#b22222" notitle

set title "rho = -0.7: tilted to the left"
plot "out/gauss2d_rho-07.txt" using 1:2 with points pt 7 ps 0.3 lc rgb "#c46210" notitle

set title "rho = 0.95: almost a straight line"
plot "out/gauss2d_rho095.txt" using 1:2 with points pt 7 ps 0.3 lc rgb "#8b0000" notitle

unset multiplot
unset output

print "plots are ready: out/gauss1d.png, out/gauss2d_sigma.png, out/gauss2d_rho.png"

set terminal qt size 760,620 font 'Verdana,9'
set title "One-dimensional Gaussian: sample histogram and theoretical curve"
set size noratio
set xlabel "value"
set ylabel "share of the sample"
set xrange [-4:4]
set key top right
plot "out/gauss1d.txt" using (bin * floor($1 / bin) + bin / 2):(1.0 / (n * bin)) \
         smooth freq with boxes lc rgb "#4a90d9" title "histogram", \
     density(x) with lines lw 2 lc rgb "#b22222" title "density N(0, 1)"

pause -1 "window is open, press Enter to close it"
