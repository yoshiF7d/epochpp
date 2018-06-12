COMPILER = gcc
CFLAGS = -O3 -g
LIBS = ../my_class/lib/libData_loadSDF.a 
INCLUDE = -I ../my_class/inc 
LDFLAGS = -lm #-lrt
TARGET1 = sdftsp

OBJDIR = ./obj
SRCDIR = ./src
VPATH =

COMDIR = ../my_class/
COMOBJNAMES = LinkedList.o Data.o utilities.o
COMOBJS = $(addprefix $(COMDIR)/obj/,$(COMOBJNAMES))

SRC1 = $(SRCDIR)/sdftsp.c
OBJS1 = $(addprefix $(OBJDIR)/,$(notdir $(SRC1:.c=.o)))

$(TARGET1): $(OBJS1) $(COMOBJS) $(LIBS)
	$(COMPILER) -o $@ $^ $(LDFLAGS)

$(COMOBJS) :
	cd $(COMDIR) && make

$(LIBS) :
	cd $(COMDIR)/lib && make
	
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

.POHNY : clean cleanall cleanlib
clean :
	rm -f $(OBJS1) $(TARGET1) 
cleanall:
	cd $(COMDIR) && make clean
cleanlib :
	cd $(COMDIR)/lib/Data_loadSDF && make clean
