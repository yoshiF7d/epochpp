COMPILER = gcc
CFLAGS = -O3 -g
LIBS = 
INCLUDE = 
LDFLAGS = -lm
TARGET = sdfjoule

OBJDIR = ./obj
SRCDIR = ./src
VPATH =

SOURCENAMES = sdfjoule.c Data.c utilities.c

SOURCES = $(addprefix $(SRCDIR)/, $(SOURCENAMES))
OBJECTS = $(addprefix $(OBJDIR)/, $(SOURCENAMES:.c=.o))

$(TARGET): $(OBJECTS) $(LIBS)
	$(COMPILER) -o $@ $^ $(LDFLAGS)
	
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(OBJDIR)/sdfjoule.o : $(SRCDIR)/Data.h $(SRCDIR)/utilities.h
$(OBJDIR)/Data.o : $(SRCDIR)/Data.h $(SRCDIR)/utilities.h
$(OBJDIR)/utilities.o : $(SRCDIR)/utilities.h

.POHNY : clean
clean :
	rm -f $(OBJECTS) $(TARGET) 
