for file in hist/*.txt
	do
	echo plotting ${file}
		name=${file%.txt}
		echo name : ${name}
	gnuplot -e \
	"set term png truecolor enhanced size 820, 510;\
	set xl 'Energy [keV]' font 'Helvetica,30' offset -2,0;\
	set yl 'Count [a.u]' font 'Helvetica,30';\
	set lmargin at screen 0.15;\
	set rmargin at screen 0.95;\
	set bmargin at screen 0.15;\
	set tmargin at screen 0.95;\
	set logscale y;\
	plot '${file}' notitle">${name}.png
done