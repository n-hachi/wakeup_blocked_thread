CXX = g++
LDFLAGS = -pthread
CPPFLAGS = -MMD -MP
TARGET = sample.exe
SRCS := $(shell find . -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:.c=.o)
DEPS := $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)
.cpp.o:
	$(CXX) -c $<

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS)
