for file in polar2d/*.txt
	do
		echo plotting ${file}
		name=${file%.txt}
		echo name : ${name}
		gnuplot -e \
		"set term png truecolor enhanced size 820, 510;\
		set xl 'Angle [deglee]' font 'Helvetica,26' offset 0,-1;\
		set yl 'Energy [keV]' font 'Helvetica,26' offset -2,0;\
        set xr[%e:%e];\
		set yr[%e:%e];\
        set tics font 'Helvetica,14';\
		set lmargin at screen 0.17;\
		set rmargin at screen 0.8;\
		set bmargin at screen 0.2;\
		set tmargin at screen 0.95;\
		set pm3d map;\
		set palette rgb 21,22,23;\
		splot '${file}' notitle">${name}.png
done