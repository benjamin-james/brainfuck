CC = gcc
CFLAGS += -W -Wall -Werror -g -O2
SOURCES = common.o compiler.o main.o
DESTDIR = /
PREFIX = /usr/local

LOCATION = $(addprefix $(DESTDIR), $(addprefix $(PREFIX), /bin))
all: bfc test

bfc: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $@
test:

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	$(RM) $(SOURCES)
	$(RM) bfc
install: bfc
	mkdir -p $(LOCATION)
	install -c bfc $(LOCATION)
doc:
	doxygen Doxyfile
