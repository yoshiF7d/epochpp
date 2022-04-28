COMPILER = gcc
CFLAGS = -O3 -g
LIBS = ../my_class/lib/libData_loadSDF.a ../my_class/lib/libData_FFT.a
INCLUDE = -I ../my_class/inc 
LDFLAGS = -lm #-lrt
TARGET1 = sdfft
TARGET2 = ffttest

OBJDIR = ./obj
SRCDIR = ./src
VPATH =

COMDIR = ../my_class/
COMOBJNAMES = Data.o LinkedList.o utilities.o
COMOBJS = $(addprefix $(COMDIR)/obj/,$(COMOBJNAMES))

SRC1 = $(SRCDIR)/sdfft.c
OBJS1 = $(addprefix $(OBJDIR)/,$(notdir $(SRC1:.c=.o)))

SRC2 = $(SRCDIR)/ffttest.c
OBJS2 = $(addprefix $(OBJDIR)/,$(notdir $(SRC2:.c=.o)))


$(TARGET1): $(OBJS1) $(COMOBJS) $(LIBS)
	$(COMPILER) -o $@ $^ $(LDFLAGS)

$(TARGET2): $(OBJS2) $(COMOBJS) $(LIBS)
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
