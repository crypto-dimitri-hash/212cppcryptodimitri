set terminal pngcairo size 1000,800 font 'Verdana,10'
set grid
set xlabel "x"
set ylabel "y"
set size ratio -1
load 'out/range.gp'

set output "out/points_clusters.png"
set title "Gaussian points coloured by cluster"
unset key
set palette defined (0 "#1f4e79", 1 "#4a90d9", 2 "#7ab648", 3 "#e0a030", \
                     4 "#b22222", 5 "#7b4ea3", 6 "#2f9e8f")
set cblabel "cluster"
plot "out/points.txt" using 1:2:5 with points pt 7 ps 0.7 palette notitle
unset output

set output "out/points_grid.png"
set title "Points and the grid of blocks"
unset colorbox
plot "out/grid_lines.txt" using 1:2 with lines lw 1 lc rgb "#c0c0c0" notitle, \
     "out/points.txt" using 1:2:5 with points pt 7 ps 0.6 palette notitle
unset output

print "plots are ready: out/points_clusters.png, out/points_grid.png"

set terminal qt size 760,620 font 'Verdana,9'
set title "Gaussian points coloured by cluster"
set colorbox
plot "out/grid_lines.txt" using 1:2 with lines lw 1 lc rgb "#c0c0c0" notitle, \
     "out/points.txt" using 1:2:5 with points pt 7 ps 0.7 palette notitle

pause -1 "window is open, press Enter to close it"
