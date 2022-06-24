editor: editor.o
	$(CC) editor.c -o editor -Wall -Wextra -pedantic -std=c99

clean:
	rm -rf $(TARGET) *.o
	rm editor