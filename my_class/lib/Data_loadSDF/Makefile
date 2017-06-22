COMPILER = gcc
CFLAGS = -g -O3
LDFLAGS = -lm
LIBS =
INCLUDE = -I ~/Documents/epochmate/my_class/inc
SRCDIR = ./src
OBJDIR = ./obj
TARGET = ./lib/libData_loadSDF.a

COMOBJDIR = ~/Documents/epochmate/my_class/obj
COMOBJNAMES = utilities.o LinkedList.o Data.o
COMOBJS = $(addprefix $(COMOBJDIR)/,$(COMOBJNAMES))

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(addprefix $(OBJDIR)/,$(notdir $(SRCS:.c=.o)))

.PHONY : test clean cleancom

$(TARGET) : $(OBJS)
	ar rcs $@ $^	

$(COMOBJS) :
	cd $(COMOBJDIR) && make

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

test:
	make test/bin/readsdf
	make test/bin/readsdflist

test/bin/readsdf: test/src/readsdf.c $(TARGET) $(COMOBJS)
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ $^ 
test/bin/readsdflist: test/src/readsdflist.c $(TARGET) $(COMOBJS)
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ $^ 

clean :
	rm -f $(OBJDIR)/*.o
	rm -f ./lib/*.a
