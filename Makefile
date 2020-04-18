NEO	?= ../neo
CC	= gcc
RM	= rm -fr

OUT	= split-expando

CFLAGS	+= -DDEBUG
CFLAGS	+= -Wall
CFLAGS	+= -Wno-unused-parameter
CFLAGS	+= -g
CFLAGS	+= -O0
CFLAGS	+= -I$(NEO)
CFLAGS	+= -fno-omit-frame-pointer

LDFLAGS	+= -L$(NEO) -lmutt -lpcre2-8

# CFLAGS	+= -fsanitize=undefined
# LDFLAGS	+= -fsanitize=undefined

# CFLAGS	+=-fsanitize=address -fsanitize-recover=address
# LDFLAGS	+=-fsanitize=address -fsanitize-recover=address

all:	$(NEO) $(OUT)

split-expando: split-expando.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

clean:
	$(RM) $(OUT)

test:	$(OUT) force
	-./$(OUT) orange peach renault seat 

force:

