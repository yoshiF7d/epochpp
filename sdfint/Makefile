COMPILER = gcc
CFLAGS = -O3 -g
LIBS = ../my_class/lib/libArray_loadSDF.a 
INCLUDE = -I ../my_class/inc 
LDFLAGS = -lm
TARGET1 = sdfintb
TARGET2 = sdfint
TARGET3 = sdfensmall
TARGET4 = sdfints

OBJDIR = ./obj
SRCDIR = ./src
VPATH =

COMDIR = ../my_class/
COMOBJNAMES = LinkedList.o Array.o utilities.o
COMOBJS = $(addprefix $(COMDIR)/obj/,$(COMOBJNAMES))

SRC1 = $(SRCDIR)/sdfintb.c
SRC2 = $(SRCDIR)/sdfint.c
SRC3 = $(SRCDIR)/sdfensmall.c
SRC4 = $(SRCDIR)/sdfints.c

OBJS1 = $(addprefix $(OBJDIR)/,$(notdir $(SRC1:.c=.o)))
OBJS2 = $(addprefix $(OBJDIR)/,$(notdir $(SRC2:.c=.o)))
OBJS3 = $(addprefix $(OBJDIR)/,$(notdir $(SRC3:.c=.o)))
OBJS4 = $(addprefix $(OBJDIR)/,$(notdir $(SRC4:.c=.o)))

all:
	make $(TARGET1) ; make $(TARGET2) ; make $(TARGET3) ; make $(TARGET4)

$(TARGET1): $(OBJS1) $(COMOBJS) $(LIBS)
	echo $(OBJS1)
	echo $(COMOBJS)
	$(COMPILER) -o $@ $^ $(LDFLAGS)

$(TARGET2): $(OBJS2) $(COMOBJS) $(LIBS)
	echo $(OBJS2)
	echo $(COMOBJS)
	$(COMPILER) -o $@ $^ $(LDFLAGS)

$(TARGET3): $(OBJS3) $(COMOBJS) $(LIBS)
	echo $(OBJS3)
	echo $(COMOBJS)
	$(COMPILER) -o $@ $^ $(LDFLAGS) 

$(TARGET4): $(OBJS4) $(COMOBJS) $(LIBS)
	echo $(OBJS4)
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
	rm -f $(OBJS1) $(TARGET1) $(OBJS2) $(TARGET2) $(OBJS3) $(TARGET3) $(OBJS4) $(TARGET4)
cleanall:
	cd $(COMDIR) && make clean
cleanlib :
	cd $(COMDIR)/lib/Array_loadSDF && make clean
