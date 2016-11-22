CC=gcc
CFLAGS=-Wall -Wextra -Werror -O2
NAME=init

all: $(NAME)

$(NAME): init.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(NAME) *.o

.PHONY: clean
