COMPILER = gcc
CFLAGS = -O3 -g -fopenmp
LIBS =
INCLUDE = 
LDFLAGS = -lm
TARGET2 = sdftrace
OBJDIR = ./obj
SRCDIR = ./src
VPATH =

SOURCENAMES2 = sdftrace.c Data.c utilities.c

SOURCES2 = $(addprefix $(SRCDIR)/, $(SOURCENAMES2))
OBJECTS2 = $(addprefix $(OBJDIR)/, $(SOURCENAMES2:.c=.o))

$(TARGET2): $(OBJECTS2) $(LIBS)
	$(COMPILER) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(OBJDIR)/sdftrace.o : $(SRCDIR)/Data.h $(SRCDIR)/utilities.h
$(OBJDIR)/Data.o : $(SRCDIR)/Data.h $(SRCDIR)/utilities.h
$(OBJDIR)/utilities.o : $(SRCDIR)/utilities.h

.POHNY : clean
clean :
	rm -f $(OBJECTS2) $(TARGET2) 
