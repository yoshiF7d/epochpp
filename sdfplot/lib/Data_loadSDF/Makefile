COMPILER = gcc
CFLAGS = -O3
LIBS =
TARGET = ./bin/readsdf
TARGET2 = ./bin/readsdflist
LIBTARGET = ./lib/libData_loadSDF.a
LDFLAGS =

OBJDIR = ./obj
SRCDIR = ./src

LIBSRCNAMES = utilities.c LinkedList.c Data.c Data_loadSDF.c sdf_input_cartesian.c sdf_input_point.c sdf_control.c sdf_input_station.c sdf_input.c sdf_util.c
SRCNAMES = readsdf.c utilities.c LinkedList.c Data.c Data_loadSDF.c sdf_input_cartesian.c sdf_input_point.c sdf_control.c sdf_input_station.c sdf_input.c sdf_util.c
SRCNAMES2 = readsdflist.c utilities.c LinkedList.c Data.c Data_loadSDF.c sdf_input_cartesian.c sdf_input_point.c sdf_control.c sdf_input_station.c sdf_input.c sdf_util.c
LIBSRCS = $(addprefix $(SRCDIR)/, $(LIBSRCNAMES))
LIBOBJS = $(addprefix $(OBJDIR)/, $(LIBSRCNAMES:.c=.o))
SRCS = $(addprefix $(SRCDIR)/, $(SRCNAMES))
OBJS = $(addprefix $(OBJDIR)/, $(SRCNAMES:.c=.o))
SRCS2 = $(addprefix $(SRCDIR)/, $(SRCNAMES2))
OBJS2 = $(addprefix $(OBJDIR)/, $(SRCNAMES2:.c=.o))


$(TARGET) : $(OBJS)
	$(COMPILER) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TARGET2) : $(OBJS2)
	$(COMPILER) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILER) $(CFLAGS) -o $@ -c $<

$(LIBTARGET) : $(LIBOBJS)
	ar rcs $(LIBTARGET) $(OBJDIR)/Data_loadSDF.o $(OBJDIR)/sdf_control.o $(OBJDIR)/sdf_input_cartesian.o $(OBJDIR)/sdf_input_point.o $(OBJDIR)/sdf_input_station.o $(OBJDIR)/sdf_input.o $(OBJDIR)/sdf_util.o

lib :
	make $(LIBTARGET)	
$(OBJDIR)/utilities.o : $(SRCDIR)/utilities.h
$(OBJDIR)/Data.o : $(SRCDIR)/Data.h
$(OBJDIR)/Data_loadSDF.o : $(SRCDIR)/Data_loadSDF.h
$(OBJDIR)/sdf_input_cartesian.o : $(SRCDIR)/sdf.h
$(OBJDIR)/sdf_input_point.o : $(SRCDIR)/sdf.h


.PHONY : clean lib
clean :
	rm -f $(OBJDIR)/*.o
	rm -f $(TARGET)
cleanlib :
	rm -f $(LIBTARGET)
