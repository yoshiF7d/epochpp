COMPILER = gcc
CFLAGS = -O3 -g
LIBS =  
INCLUDE = -I ../my_class/inc 
LDFLAGS = -lm
TARGET = maskelipse
TARGET2 = maskcircles

OBJDIR = ./obj
SRCDIR = ./src
VPATH =

COMDIR = ../my_class/
COMOBJNAMES = Data.o LinkedList.o utilities.o
COMOBJS = $(addprefix $(COMDIR)/obj/,$(COMOBJNAMES))

SRC1 = $(SRCDIR)/maskelipse.c
OBJS1 = $(addprefix $(OBJDIR)/,$(notdir $(SRC1:.c=.o)))
SRC2 = $(SRCDIR)/maskcircles.c
OBJS2 = $(addprefix $(OBJDIR)/,$(notdir $(SRC2:.c=.o)))

$(TARGET): $(OBJS1) $(COMOBJS) $(LIBS)
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

.POHNY : clean cleanall 
clean :
	rm -f $(OBJS1) $(TARGET1) 
cleanall:
