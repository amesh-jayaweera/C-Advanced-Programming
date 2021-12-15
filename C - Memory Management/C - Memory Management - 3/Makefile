CC = clang
CFLAGS = -Wall -DNDEBUG

PROG = regions
HDRS = regions.h region_list.h block_list.h
SRCS = regions.c region_list.c block_list.c main3.c

OBJDIR = object
OBJS = $(OBJDIR)/regions.o $(OBJDIR)/region_list.o $(OBJDIR)/block_list.o $(OBJDIR)/main3.o

$(PROG): $(OBJS) $(OBJDIR)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROG)

$(OBJDIR)/regions.o: regions.c $(HDRS) $(OBJDIR)
	$(CC) $(CFLAGS) -c regions.c -o $(OBJDIR)/regions.o

$(OBJDIR)/region_list.o: region_list.c $(HDRS) $(OBJDIR)
	$(CC) $(CFLAGS) -c region_list.c -o $(OBJDIR)/region_list.o

$(OBJDIR)/block_list.o: block_list.c $(HDRS) $(OBJDIR)
	$(CC) $(CFLAGS) -c block_list.c -o $(OBJDIR)/block_list.o

$(OBJDIR)/main3.o: main3.c $(HDRS) $(OBJDIR)
	$(CC) $(CFLAGS) -c main3.c -o $(OBJDIR)/main3.o

$(OBJDIR):
	mkdir $(OBJDIR)

clean:
	rm -f $(PROG) $(OBJS)
