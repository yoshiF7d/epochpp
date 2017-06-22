#define PLOTESPC \
"mkdir png\nfor file in data/*.txt\n\
\tdo\n\
\techo plotting ${file}\n\
\t\ttmp=${file##*/}\n\
\t\tname=${tmp%%.txt}\n\
\t\techo name : ${name}\n\
\tgnuplot -e \\\n\
\t\"set term png truecolor enhanced size 820, 510;\\\n\
\tset xl \'Energy [keV]\' font \'Helvetica,26\' offset 0,-1;\\\n\
\tset yl \'Count [a.u]\' font \'Helvetica,26\' offset -2,0;\\\n\
\tset tics font \'Helvetica,14\';\\\n\
\tset xr[%e:%e];\\\n\
\tset format x \\\"%%g\\\";\\\n\
\tset format y \\\"10^{%%L}\\\";\\\n\
\tset lmargin at screen 0.17;\\\n\
\tset rmargin at screen 0.95;\\\n\
\tset bmargin at screen 0.18;\\\n\
\tset tmargin at screen 0.95;\\\n\
\tset logscale y;\\\n\
\tplot \'${file}\' notitle\">png/${name}.png\n\
done"
#define PLOTPOLAR2D \
"mkdir png\nfor file in data/*.txt\n\
\tdo\n\
\t\techo plotting ${file}\n\
\t\ttmp=${file##*/}\n\
\t\tname=${tmp%%.txt}\n\
\t\techo name : ${name}\n\
\t\tgnuplot -e \\\n\
\t\t\"set term png truecolor enhanced size 820, 510;\\\n\
\t\tset xl \'Angle [deglee]\' font \'Helvetica,26\' offset 0,-1;\\\n\
\t\tset yl \'Energy [keV]\' font \'Helvetica,26\' offset -2,0;\\\n\
\t\tset yr[%e:%e];\\\n\
\t\tset tics font \'Helvetica,14\';\\\n\
\t\tset lmargin at screen 0.17;\\\n\
\t\tset rmargin at screen 0.8;\\\n\
\t\tset bmargin at screen 0.2;\\\n\
\t\tset tmargin at screen 0.95;\\\n\
\t\tset pm3d map;\\\n\
\t\tset palette rgb 21,22,23;\\\n\
\t\tsplot \'${file}\' notitle\">png/${name}.png\n\
done"
#define PLOTPOLAR \
"mkdir png\nfor file in data/*.txt\n\
\tdo\n\
\t\techo plotting ${file}\n\
\t\ttmp=${file##*/}\n\
\t\tname=${tmp%%.txt}\n\
\t\techo name : ${name}\n\
\t\tgnuplot -e \\\n\
\t\t\"set term png truecolor enhanced size 510, 510;\\\n\
\t\tset size square;\\\n\
\t\tset xl \'%s\' font \'Helvetica,26\' offset 0,-1;\\\n\
\t\tset xr[%e:%e];\\\n\
\t\tset yr[%e:%e];\\\n\
\t\tset tics font \'Helvetica,14\';\\\n\
\t\tset lmargin at screen 0.17;\\\n\
\t\tset rmargin at screen 0.8;\\\n\
\t\tset bmargin at screen 0.2;\\\n\
\t\tset tmargin at screen 0.95;\\\n\
\t\tset pm3d map;\\\n\
\t\tset palette rgb 21,22,23;\\\n\
\t\tset angles degrees;\\\n\
\t\tset map cylindrical;\\\n\
\t\tset parametric;\\\n\
\t\tsplot \'${file}\' u 2:3:1 notitle\">png/${name}.png\n\
done"
