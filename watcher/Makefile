
TARGET = watcher.exe

SRCS = watcher.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

%.o: %.cpp
	c++ -c $^

$(TARGET): $(OBJS)
	c++ $(OBJS) -o $@

clear:
	rm -rf $(OBJS)

aclear: clear
	rm -rf watcher.exe