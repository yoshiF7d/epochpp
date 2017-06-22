COMPILER = gcc
CFLAGS = -O3 -g -fopenmp
LIBS =
INCLUDE = 
LDFLAGS = -lm
TARGET1 = sdfphase

OBJDIR = ./obj
SRCDIR = ./src
VPATH =

SOURCENAMES = sdfphase.c 

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(OBJECTS1) $(LIBS)
	$(COMPILER) $(CFLAGS) -o $@ $^ $(LDFLAGS)
$(TARGET2): $(OBJECTS2) $(LIBS)
	$(COMPILER) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(OBJDIR)/sdfphase.o : $(SRCDIR)/Data.h $(SRCDIR)/utilities.h
$(OBJDIR)/sdfphase2.o : ${SRCDIR}/Data.h ${SRCDIR}/utilities.h
$(OBJDIR)/Data.o : $(SRCDIR)/Data.h $(SRCDIR)/utilities.h
$(OBJDIR)/utilities.o : $(SRCDIR)/utilities.h

.POHNY : clean
clean :
	rm -f $(OBJECTS1) $(TARGET1) 
	rm -f $(OBJECTS2) $(TARGET2)
