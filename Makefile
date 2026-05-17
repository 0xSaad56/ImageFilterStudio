CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wno-unused-parameter -O2
TARGET   = ImageFilterStudio

SRCS = main.cpp \
       Pixel.cpp \
       Image.cpp \
       Filters.cpp \
       FilterSession.cpp \
       FileManager.cpp \
       User.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo ""
	@echo "  Build successful!  Run: ./$(TARGET)"
	@echo ""

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
