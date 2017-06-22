COMPILER = gcc
CFLAGS = -g -O3
LIBS =  
LDFLAGS = -lm 

OBJDIR = ./obj
SRCDIR = ./src
INCDIR = ./inc

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(addprefix $(OBJDIR)/, $(notdir $(SRCS:.c=.o)))

all :
	make obj && make lib 
obj : $(OBJS)
lib : 
	cd ./lib && make

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILER) $(CFLAGS) -I $(INCDIR) -o $@ -c $<

.PHONY : clean all lib obj
clean :
	rm -f $(OBJDIR)/*.o	
