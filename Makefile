# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -std=c1x -pedantic
LDFLAGS = -lpthread

# Имя исполняемого файла
TARGET = program

# Исходные файлы
SRCS = src/main.c
OBJS = $(SRCS:.c=.o)

# Правило по умолчанию
all: $(TARGET)

# Сборка исполняемого файла
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Компиляция объектных файлов
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Очистка
clean:
	rm -f $(TARGET) $(OBJS) file1.txt file2.txt file3.txt file4.txt

# Пересборка
rebuild: clean all

# Запуск программы
run: $(TARGET)
	./$(TARGET)

# Отладочная сборка
debug: CFLAGS += -g -DDEBUG
debug: rebuild

# Зависимости
program.o: program.c