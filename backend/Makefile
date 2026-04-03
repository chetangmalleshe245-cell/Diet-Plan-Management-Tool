CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
SRC_DIR  = src
INC_DIR  = include
SOURCES  = $(wildcard $(SRC_DIR)/*.cpp)
TARGET   = diet_api.cgi

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -o $@ $^

# Quick test (runs as a normal program, not CGI)
run: $(TARGET)
	QUERY_STRING="action=request_otp&email=test@example.com" ./$(TARGET)

clean:
	rm -f $(TARGET)
