dir=${1:-.}
pname=$2
axis=${3:-1}
emin=${4:--1}
emax=${5:--1}
mass=${6:-9.109383e-31}
ebinc=${7:-512}
pmin=${8:--1}
pmax=${9:--1}
pebinc=${10:-128}
pabinc=${11:-128}
area=${12:-1}
piecutn=${13:-1}
step=${14:-1}

count=0
for file in ${dir}/*.sdf
do
	mod=$(($count % $step))
	if [ $mod -eq 0 ]; then
 	   echo sdfprobe ${file} ${pname} ${axis} ${emin} ${emax} ${mass} ${ebinc} ${pmin} ${pmax} ${pebinc} ${pabinc} ${area} ${piecutn} 
 	   sdfprobe ${file} ${pname} ${axis} ${emin} ${emax} ${mass} ${ebinc} ${pmin} ${pmax} ${pebinc} ${pabinc} ${area} ${piecutn} 
	fi
	count=$(($count+1))
done
echo 'cd aeangd && ./plot.sh && cd ..
cd eangd && ./plot.sh && cd ..
cd iangd && ./plot.sh && cd ..
cd espc && ./plot.sh && cd ..
cd eiangd && {
	for a in */
	do
		cd ${a} && ./plot.sh && cd ..
	done
} && cd ..' > ${pname}/plotall.sh
chmod +x ${pname}/plotall.sh
