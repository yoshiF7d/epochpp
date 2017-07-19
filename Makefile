all :
	for dir in * ; do \
		(cd $${dir} && make) ; \
	done
clean :
	for dir in * ; do \
		(cd $${dir} && make clean) ; \
	done
