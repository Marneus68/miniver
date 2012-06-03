TARGET  := miniver
WARN    := -Wall
OFLAGS  := -O2
LDFLAGS := -lsqlite3
CC      := gcc

C_SRCS      = $(wildcard *.c)
OBJ_FILES   = $(C_SRCS:.c=.o)

%o: %c
	@echo "Compiling "$<"..."
	$(CC) -c $(OFLAGS) $< -o $@

$(TARGET): $(OBJ_FILES)
	@echo "Linking..."
	$(CC)  $(OFLAGS) $(LDFLAGS) -o $@ $(OBJ_FILES)
	@echo "Done."

all: ${TARGET}

clean:
	@echo "Cleaning..."
	rm -rf *.o

mrproper: clean
	rm -rf ${TARGET}

install: all
	install -Dm755 ${TARGET} $(DESTDIR)/usr/bin/${TARGET}

uninstall:
	rm -f $(DESTDIR)/usr/bin/${TARGET}

