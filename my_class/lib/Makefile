LIBDIRS = $(wildcard */)
.POHNY : all clean

all :
	for var in $(LIBDIRS) ; do \
		cd $$var && make ; cd ../ && ln -sf $$var/lib/*.a . ;\
	done
clean :
	rm -f *.a
	for var in $(LIBDIRS) ; do \
		cd $$var && make clean ; cd ../ ;\
	done
