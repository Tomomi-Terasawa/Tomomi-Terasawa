TARGET = graphic5.exe

OBJS   = 
LIBS   = -L$(HOME)/lib -lADDA -lXG -lXMMenu -lXMDialog -lFFT \
         -L/usr/X11R6/lib -lX11 -lwinmm -lm
CFLAGS = -I$(HOME)/include -O
CC     = gcc
SWITCHES = $(CFLAGS)

all    : $(TARGET)

%.exe  : %.o $(OBJS)
	$(CC) $(SWITCHES) $< $(OBJS) $(LIBS) -o $@
$(OBJS):

clean  :
	rm -f *~ *.o $(TARGET)
