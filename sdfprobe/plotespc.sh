for file in espc/*.txt
	do
	echo plotting ${file}
		name=${file%.txt}
		echo name : ${name}
	gnuplot -e \
	"set term png truecolor enhanced size 820, 510;\
	set xl 'Energy [keV]' font 'Helvetica,26' offset 0,-1;\
	set yl 'Count [a.u]' font 'Helvetica,26' offset -2,0;\
	set tics font 'Helvetica,14';\
    set xr[%e:%e];\
	set yr[%e:%e];\
    set format x \"%g\";\
    set format y \"10^{%L}\";\
	set lmargin at screen 0.17;\
	set rmargin at screen 0.95;\
	set bmargin at screen 0.18;\
	set tmargin at screen 0.95;\
	set logscale y;\
	plot '${file}' notitle">${name}.png
done