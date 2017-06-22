COMPILER = gcc
CFLAG = -g -O3
LIBS =
INCLUDE = -I ../mylib/include
TARGET = ./bin/learn
SRCDIR = ./src
OBJDIR = ./obj
LIBSRCDIR = ../mylib/src
LIBOBJDIR = ../mylib/obj

LDFLAGS = -lm

SRCNAMES = learn.c
SRCS = $(addprefix $(SRCDIR)/, $(SRCNAMES))
OBJS = $(addprefix $(OBJDIR)/, $(SRCNAMES:.c=.o))

OBJS += $(LIBOBJDIR)/utilities.o

$(TARGET) : $(OBJS) $(LIBOBJS)
	$(COMPILER) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(LIBOBJDIR)/%.o : $(LIBSRCDIR)/%.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

.POHNY : clean
clean :
	rm -f $(LIBOBJDIR)/*.o $(OBJDIR)/*.o
