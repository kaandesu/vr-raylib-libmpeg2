APPNAME := $(shell basename `pwd`)

LDFLAGS := -L/opt/homebrew/lib -lmpeg2convert -lraylib -lm \
            -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo \
            $(shell pkg-config --libs libmpeg2)

CFLAGS := -Wfatal-errors -pedantic -Wall -Wextra -Werror -std=c99 \
           -I /opt/homebrew/include \
           $(shell pkg-config --cflags libmpeg2) \
					 -O3

SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=obj/%.o)

CC = clang

$(APPNAME): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: debug release clean style run

run: $(APPNAME)
	@ ./$(APPNAME)


clean:
	rm -f obj/* $(APPNAME)

