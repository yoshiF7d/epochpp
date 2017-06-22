COMPILER = gcc
CFLAGS = -O3 -g
LIBS = lib/libData_loadSDF.a
INCLUDE = 
LDFLAGS = -lm 
TARGET = sdfprobe

OBJDIR = ./obj
SRCDIR = ./src
VPATH =

SOURCENAMES = sdfprobe.c Data.c LinkedList.c utilities.c

SOURCES = $(addprefix $(SRCDIR)/, $(SOURCENAMES))
OBJECTS = $(addprefix $(OBJDIR)/, $(SOURCENAMES:.c=.o))

$(TARGET): $(OBJECTS) $(LIBS)
	$(COMPILER) -o $@ $^ $(LDFLAGS)
	
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(LIBS) :
	cd lib && make

$(OBJDIR)/sdfprobe.o : $(SRCDIR)/Data.h $(SRCDIR)/utilities.h
$(OBJDIR)/Data.o : $(SRCDIR)/Data.h $(SRCDIR)/utilities.h
$(OBJDIR)/LinkedList.o : $(SRCDIR)/LinkedList.h $(SRCDIR)/utilities.h
$(OBJDIR)/utilities.o : $(SRCDIR)/utilities.h

.POHNY : clean cleanlib
clean :
	rm -f $(OBJECTS) $(TARGET)
cleanlib :
	cd lib && make clean
