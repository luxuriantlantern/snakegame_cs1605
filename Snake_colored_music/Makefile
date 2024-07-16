# 编译器
CC = g++

# 编译器选项
CFLAGS = -Wall -Wextra -std=c++11 -IC:/mingw64/pdcurses_tutorial

# 链接器选项
LDFLAGS = -LC:/mingw64/pdcurses_tutorial -lpdcurses -lwinmm

# 目标文件
TARGET = snakegame

# 源文件
SRCS = main.cpp snake.cpp game.cpp

# 对象文件
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /f $(OBJS) $(TARGET)

.PHONY: all clean
