CC = gcc
CFLAGS = -Wall -Wextra -Werror -g3 -fsanitize=address
TARGET = my_ls

all: $(TARGET)

$(TARGET): my_ls.c
	$(CC) $(CFLAGS) -o $(TARGET) my_ls.c

clean:
	rm -f $(TARGET)

fclean: clean
	rm -f my_ls.output ls.output

re: fclean all

.PHONY: all clean fclean re
