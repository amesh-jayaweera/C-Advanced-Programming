CC = clang
CFLAGS = -Wall -DNDEBUG

PROG = regions
HDRS = regions.h region_list.h block_list.h
SRCS = regions.c region_list.c block_list.c main1.c

OBJDIR = object
OBJS = $(OBJDIR)/regions.o $(OBJDIR)/region_list.o $(OBJDIR)/block_list.o $(OBJDIR)/main1.o

$(PROG): $(OBJS) $(OBJDIR)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROG)

$(OBJDIR)/regions.o: regions.c $(HDRS) $(OBJDIR)
	$(CC) $(CFLAGS) -c regions.c -o $(OBJDIR)/regions.o

$(OBJDIR)/region_list.o: region_list.c $(HDRS) $(OBJDIR)
	$(CC) $(CFLAGS) -c region_list.c -o $(OBJDIR)/region_list.o

$(OBJDIR)/block_list.o: block_list.c $(HDRS) $(OBJDIR)
	$(CC) $(CFLAGS) -c block_list.c -o $(OBJDIR)/block_list.o

$(OBJDIR)/main1.o: main1.c $(HDRS) $(OBJDIR)
	$(CC) $(CFLAGS) -c main1.c -o $(OBJDIR)/main1.o

$(OBJDIR):
	mkdir $(OBJDIR)

clean:
	rm -f $(PROG) $(OBJS)
