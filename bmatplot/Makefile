COMPILER = gcc 
CFLAGS = -O3 -g
LDFLAGS = -lm
TARGET = bmatplot

OBJDIR = ./obj
SRCDIR = ./src
COMDIR = ../my_class
COMOBJDIR = $(COMDIR)/obj
INCLUDE = -I $(COMDIR)/inc
COMOBJNAMES = utilities.o Data.o LinkedList.o
COMOBJS = $(addprefix $(COMOBJDIR)/,$(COMOBJNAMES)) 

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(addprefix $(OBJDIR)/, $(notdir $(SRCS:.c=.o)))

$(TARGET): $(OBJS) $(COMOBJS) 
	$(COMPILER) -o $@ $^ $(PLUGINS) $(LDFLAGS)	

$(COMOBJS):
	cd $(COMDIR) && make
$(LIBS):
	cd $(COMLIBDIR) && make

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

.POHNY: clean cleanall cleanlib
clean:
	rm -f $(OBJS) $(TARGET)
cleanall:
	cd $(COMDIR) && make clean
cleanlib:
	cd $(COMLIBDIR) && make clean
