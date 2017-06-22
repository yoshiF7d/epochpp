.PHONY : eqsolv3 all learn clean print
all:
	./tool/bin/learn ./knowledge ./core/src/knowledge.c ./core/include/knowledge.h ./core/src/data.c ./core/include/data.h 0
	cd ./core && make
debug:
	sed -i "" -e 's:.*#define LEAKDETECT.*:#define LEAKDETECT 0:' 'mylib/include/utilities.h'
	./tool/bin/learn ./knowledge ./core/src/knowledge.c ./core/include/knowledge.h ./core/src/data.c ./core/include/data.h 0
	cd ./core && make ../bin/eqsolv3d
debug0:
	sed -i "" -e 's:.*#define LEAKDETECT.*:#define LEAKDETECT 0:' 'mylib/include/utilities.h'
	./tool/bin/learn ./knowledge ./core/src/knowledge.c ./core/include/knowledge.h ./core/src/data.c ./core/include/data.h 1
	cd ./core && make ../bin/eqsolv3d
debug1:
	sed -i "" -e 's:.*#define LEAKDETECT.*:#define LEAKDETECT 1:' 'mylib/include/utilities.h'
	./tool/bin/learn ./knowledge ./core/src/knowledge.c ./core/include/knowledge.h ./core/src/data.c ./core/include/data.h 1
	cd ./core && make ../bin/eqsolv3d
release:
	sed -i "" -e 's:.*#define LEAKDETECT.*://#define LEAKDETECT 0:' 'mylib/include/utilities.h'	
	./tool/bin/learn ./knowledge ./core/src/knowledge.c ./core/include/knowledge.h ./core/src/data.c ./core/include/data.h 0
	cd ./core && make
eqsolv3:
	cd ./core && make
learn:
	sed -i "" -e 's:.*#define LEAKDETECT.*:#define LEAKDETECT 0:' 'mylib/include/utilities.h'
	cd ./tool && make
learn_debug0:
	sed -i "" -e 's:.*#define LEAKDETECT.*:#define LEAKDETECT 0:' 'mylib/include/utilities.h'
	cd ./tool && make
clean: 	
	-rm -f ./knowledge/*/src/*.c
	-rm -f ./knowledge/*/obj/*.o
	cd ./core && make clean
	cd ./tool && make clean
print:
	cd ./core && make print
