CC=gcc

CFLAGS=-g -Wall
INC_PATH=-I ./include
LIB_PATH=
LIBS=

all:libnsprx.so

OBJS+=src/prxbuffer.o
OBJS+=src/prxchannel.o
OBJS+=src/prxevent.o
OBJS+=src/prxutils.o

%.o:%.cc
	@echo [Compile] $^
	@$(CXX) $(CFLAGS) $(INC_PATH) -c $^ -o $@

%.o:%.c
	@echo [Compile] $^
	@$(CC) $(CFLAGS) $(INC_PATH) -c -fPIC $^ -o $@


libnsprx.so: $(OBJS)
	@echo [Link] $^
	$(CC) -shared $^ $(LIB_PATH) $(LIBS) -o $@

clean:
	rm -f $(OBJS)
	rm libnsprx.so
