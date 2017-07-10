COMPILER = gcc
CFLAGS = -O3
INCLUDE = -I include -I ../my_class/inc
LDFLAGS = -lreadline -lncurses -lpng
TARGET = visiter

OBJDIR = ./obj
SRCDIR = ./src
COMDIR = ../my_class
COMOBJDIR = $(COMDIR)/obj
COMLIBDIR = $(COMDIR)/lib

COMLIBNAMES = Data_loadSDF Data_loadPNG
COMLIBS = $(addprefix $(COMLIBDIR)/,$(COMLIBNAMES))
LIBS = $(addsuffix .a ,$(addprefix $(COMLIBDIR)/lib,$(COMLIBNAMES))) 

COMOBJNAMES = utilities.o ArrayList.o Data.o HashTable2.o LinkedList.o
COMOBJS = $(addprefix $(COMOBJDIR)/,$(COMOBJNAMES))

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(addprefix $(OBJDIR)/,$(notdir $(SRCS:.c=.o)))

$(TARGET) : $(OBJS) $(COMOBJS) $(LIBS)
	$(COMPILER) -o $@ $^ $(PLUGINS) $(LDFLAGS) 

$(COMOBJS) :
	cd $(COMDIR) && make

$(LIBS) :
	cd $(COMLIBDIR) && make

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

.POHNY : clean cleanall cleanlib
clean :
	rm -f $(OBJS) $(TARGET) 
cleanall:
	cd $(COMDIR) && make clean

cleanlib :
	cd $(COMLIBDIR) && make clean
