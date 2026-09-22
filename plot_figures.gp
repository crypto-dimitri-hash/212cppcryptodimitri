set terminal pngcairo size 1000,800 font 'Verdana,10'
set grid
set xlabel "x"
set ylabel "y"

load 'out/range.gp'

set output "out/plan.png"
set title "Figures on a plane: top view"
set size ratio -1
set key top left
set style fill transparent solid 0.25 border lc rgb "#1f4e79"
plot "out/plan_cones.txt"   using 1:2:3 with circles lc rgb "#4a90d9" title "cones", \
     "out/plan_circles.txt" using 1:2:3 with circles lc rgb "#7ab648" title "circles", \
     "out/plan_cones.txt"   using 1:2 with points pt 7 ps 0.8 lc rgb "#b22222" notitle, \
     "out/plan_points.txt"  using 1:2 with points pt 7 ps 1.2 lc rgb "#333333" title "points", \
     "out/plan_cones.txt"   using 1:2:(sprintf("h=%.1f", $4)) with labels \
         offset 0,1.2 font ",8" textcolor rgb "#404040" notitle
unset output

set output "out/map.png"
set title "Height map z(x, y)"
set view map
set pm3d at b
set palette defined (0 "#eef3e6", 0.2 "#8fbf5a", 0.5 "#e0c878", 0.8 "#a8743a", 1 "#ffffff")
set cblabel "height"
unset key
splot "out/field.txt" using 1:2:3 with pm3d notitle
unset output

set output "out/landscape.png"
set title "Cone landscape z(x, y)"
unset view
set view 55, 35, 1.0, 1.0
set size ratio 1
set xyplane at 0
set zlabel "h"
set pm3d
set hidden3d
splot "out/field.txt" using 1:2:3 with pm3d notitle
unset output

set output "out/points3d.png"
set title "Figures drawn with points on their surface"
unset pm3d
unset hidden3d
set view 62, 30, 1.0, 1.0
splot "out/surface_points.txt" using 1:2:3 with points pt 7 ps 0.2 \
      lc rgb "#1f4e79" notitle
unset output

print "plots are ready: out/plan.png, out/map.png, out/landscape.png, out/points3d.png"

set terminal qt size 760,620 font 'Verdana,9'
set title "Figures on a plane: top view"
unset view
set size ratio -1
set key top left
plot "out/plan_cones.txt"   using 1:2:3 with circles lc rgb "#4a90d9" title "cones", \
     "out/plan_circles.txt" using 1:2:3 with circles lc rgb "#7ab648" title "circles", \
     "out/plan_cones.txt"   using 1:2 with points pt 7 ps 0.8 lc rgb "#b22222" notitle, \
     "out/plan_points.txt"  using 1:2 with points pt 7 ps 1.2 lc rgb "#333333" title "points", \
     "out/plan_cones.txt"   using 1:2:(sprintf("h=%.1f", $4)) with labels \
         offset 0,1.2 font ",8" textcolor rgb "#404040" notitle

pause -1 "window is open, press Enter to close it"
