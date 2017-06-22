COMPILER = gcc
CFLAGS = -O3 -g
LIBS = ../my_class/lib/libArray_loadSDF.a ../my_class/lib/libData_loadSDF.a
INCLUDE = -I ../my_class/inc 
LDFLAGS = -lm
TARGET1 = sdfwave2d
TARGET2 = sdfwave3d

OBJDIR = ./obj
SRCDIR = ./src
VPATH =

COMDIR = ../my_class/
COMOBJNAMES = LinkedList.o Data.o Array.o utilities.o
COMOBJS = $(addprefix $(COMDIR)/obj/,$(COMOBJNAMES))

SRC1 = $(SRCDIR)/sdfwave2d.c
SRC2 = $(SRCDIR)/sdfwave3d.c

OBJS1 = $(addprefix $(OBJDIR)/,$(notdir $(SRC1:.c=.o)))
OBJS2 = $(addprefix $(OBJDIR)/,$(notdir $(SRC2:.c=.o)))

$(TARGET1): $(OBJS1) $(COMOBJS) $(LIBS)
	echo $(OBJS1)
	echo $(COMOBJS)
	$(COMPILER) -o $@ $^ $(LDFLAGS)

$(TARGET2): $(OBJS2) $(COMOBJS) $(LIBS)
	echo $(OBJS2)
	echo $(COMOBJS)
	$(COMPILER) -o $@ $^ $(LDFLAGS)

$(COMOBJS) :
	cd $(COMDIR) && make

$(LIBS) :
	cd $(COMDIR)/lib && make
	
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

.POHNY : clean cleanall cleanlib
clean :
	rm -f $(OBJS) $(TARGET)
cleanall:
	cd $(COMDIR) && make clean
cleanlib :
	cd $(COMDIR)/lib/Data_loadSDF && make clean
	cd $(COMDIR)/lib/Array_loadSDF && make clean
