COMPILER = gcc
CFLAGS = -g -O3
LDFLAGS = -lm
LIBS =
INCLUDE = -I ../../inc
SRCDIR = ./src
OBJDIR = ./obj
TARGET = ./lib/libData_FFT.a

COMOBJDIR = ../../obj
COMOBJNAMES = utilities.o Data.o
COMOBJS = $(addprefix $(COMOBJDIR)/,$(COMOBJNAMES))

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(addprefix $(OBJDIR)/,$(notdir $(SRCS:.c=.o)))

.PHONY : clean cleancom

$(TARGET) : $(OBJS)
	ar rcs $@ $^	

$(COMOBJS) :
	cd $(COMOBJDIR) && make

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

clean :
	rm -f $(OBJDIR)/*.o
	rm -f ./lib/*.a
