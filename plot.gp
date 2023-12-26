# gnuplot
reset

set terminal pngcairo enhanced size 1920/2,1080/2
set grid
set key box opaque

set ylabel "|[x y z]| - 1g"
set xlabel "Samples over time"
set yrange [-0.3:0.1]

n(g) = g - 1.0

do for [g in "2g 4g 8g 16g"] {
    name = sprintf("graph-%s", g)
    out = sprintf("%s.png", name)
    stub = sprintf("data/fifo_%s_", g)
    set title sprintf("[%s FIFO] comparison of ODR setting effect on magnitude of [x y z] acc vector (- 1g)", g)
    set output out
    plot stub."50.txt" u n(4) w p ps 0.3 t "50 Hz", \
         stub."100.txt" u n(4) w p ps 0.3 t "100 Hz", \
         stub."200.txt" u n(4) w p ps 0.3 t "200 Hz", \
         stub."400.txt" u n(4) w p ps 0.3 t "400 Hz", \
}
