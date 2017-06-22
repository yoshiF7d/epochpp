COMPILER = gcc
CFLAGS = -g -O3
LIBS = 
INCLUDE = -I ../mylib/include -I ./include
TARGET = ../bin/eqsolv3
TARGET2 = ../bin/eqsolv3d
SRCDIR = ./src
OBJDIR = ./obj
INCDIR = ./include

LSRCDIR = ../mylib/src
LOBJDIR = ../mylib/obj
LINCDIR = ../mylib/include
KDIR = ../knowledge

LDFLAGS = -lm -lreadline -lncurses -lgmp -lmpfr
SRCNAMES = eqsolv3.c parser.c Expr.c SymbolTable.c knowledge.c data.c
LSRCNAMES = utilities.c HashTable3.c ArrayList.c LinkedList.c
SRCS = $(addprefix $(SRCDIR)/, $(SRCNAMES))
OBJS = $(addprefix $(OBJDIR)/, $(SRCNAMES:.c=.o))
LSRCS = $(addprefix $(LSRCDIR)/, $(LSRCNAMES))
LOBJS = $(addprefix $(LOBJDIR)/, $(LSRCNAMES:.c=.o))

LSRCS = $(addprefix $(SRCDIR)/, $(SRCNAMES))
KSRCS = $(foreach PDIR, $(wildcard $(KDIR)/*), $(wildcard $(PDIR)/src/*.c))
KOBJS = $(foreach PDIR, $(wildcard $(KDIR)/*), $(addprefix $(PDIR)/obj/, $(patsubst %.c,%.o,$(notdir $(wildcard $(PDIR)/src/*.c)))))

OBJS += $(LOBJS) 
OBJS += $(KOBJS)

$(TARGET) : $(OBJS) $(LIBOBJS)
	$(COMPILER) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TARGET2) : $(OBJS) $(LIBOBJS)
	$(COMPILER) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<	

$(LOBJDIR)/%.o : $(LSRCDIR)/%.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<	

$(KDIR)/*/obj/%.o : $(KDIR)/*/src/%.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(OBJDIR)/parser.o : $(INCDIR)/knowledge.h 
$(LOBJDIR)/utilities.o : $(LINCDIR)/utilities.h

.PHONY : clean print
clean :
	-rm -f $(LOBJDIR)/*.o $(OBJDIR)/*.o
	-rm -f $(SRCDIR)/knowledge.c $(INCDIR)/knowledge.h $(SRCDIR)/data.c $(INCDIR)/data.h
	
print : 
	echo $(KOBJS)
	echo $(KSRCS)
