COMPILER = gcc
CFLAGS = -O3 -g
LIBS = ../my_class/lib/libData_loadSDF.a
INCLUDE = -I ../my_class/inc 
LDFLAGS = -lm
TARGET = sdf1d

OBJDIR = ./obj
SRCDIR = ./src
VPATH =

SOURCENAMES = sdf1d.c Data.c LinkedList.c utilities.c

SOURCES = $(addprefix $(SRCDIR)/, $(SOURCENAMES))
OBJECTS = $(addprefix $(OBJDIR)/, $(SOURCENAMES:.c=.o))

$(TARGET): $(OBJECTS) $(LIBS)
	$(COMPILER) -o $@ $^ $(LDFLAGS) 
	
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(LIBS) :
	cd lib && make

$(OBJDIR)/Data.o : $(SRCDIR)/Data.h $(SRCDIR)/utilities.h
$(OBJDIR)/LinkedList.o : $(SRCDIR)/LinkedList.h $(SRCDIR)/utilities.h
$(OBJDIR)/utilities.o : $(SRCDIR)/utilities.h

.POHNY : clean cleanlib
clean :
	rm -f $(OBJECTS) $(TARGET) 
cleanlib :
	cd lib && make clean
