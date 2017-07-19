COMPILER = gcc
CFLAGS = -O3 -g -fopenmp
LIBS = ../my_class/lib/libData_loadSDF.a
INCLUDE = -I ../my_class/inc
LDFLAGS = -lm
TARGET = sdfphase

OBJDIR = ./obj
SRCDIR = ./src
VPATH =

COMDIR = ../my_class/
COMOBJNAMES = Data.o LinkedList.o utilities.o
COMOBJS = $(addprefix $(COMDIR)/obj/,$(COMOBJNAMES))

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(addprefix $(OBJDIR)/,$(notdir $(SRCS:.c=.o)))

$(TARGET): $(OBJS) $(COMOBJS) $(LIBS)
	$(COMPILER) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<
$(COMOBJS) :
	cd $(COMDIR) && make
$(LIBS) :
	cd $(COMDIR)/lib && make

.POHNY : clean cleanall cleanlib
clean :
	rm -f $(OBJS) $(TARGET)
cleanall:
	cd $(COMDIR) && make clean
cleanlib:
	cd $(COMDIR)/lib/Data_loadSDF && make clean
