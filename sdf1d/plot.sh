dir=${1:-.}
outdir=${2:-png}
mkdir ${outdir}
for file in ${dir}/*.txt
do
	tmp=${filepath##*/}
	tmp=${tmp%.*}
	gnuplot -e \
	"set term png truecolor enhanced size 820, 510;\
	set xl 'X [um]' font 'Helvetica,30' offset -2,0;\
	set yl 'Y a.u' font 'Helvetica,30';\
	set lmargin at screen 0.15;\
	set rmargin at screen 0.95;\
	set bmargin at screen 0.15;\
	set tmargin at screen 0.95;\
	plot '${file}' notitle">${name}.png	
done
