
TARGET = watcher

SRCS = watcher.cpp main.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

%.o: %.cpp
	c++  -c $^

$(TARGET): $(OBJS)
	c++ -static-libstdc++ $(OBJS)  -o $@

clear:
	rm -rf $(OBJS)

aclear: clear
	rm -rf watcher.exe