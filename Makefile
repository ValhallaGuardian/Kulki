# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = -I/usr/include/SFML
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
TARGET = kulki

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Default target
all: $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBS)

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Rebuild everything
rebuild: clean all

# Run the program
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Release build (default is already optimized)
release: CXXFLAGS += -DNDEBUG
release: $(TARGET)

# Install dependencies (Ubuntu/Debian)
install-deps:
	sudo apt update && sudo apt install -y \
	libsfml-dev \
	build-essential

# Show help
help:
	@echo "Available targets:"
	@echo "  all	   - Build the project (default)"
	@echo "  clean	 - Remove build artifacts"
	@echo "  rebuild   - Clean and build"
	@echo "  run	   - Build and run the program"
	@echo "  debug	 - Build with debug symbols"
	@echo "  release   - Build optimized release"
	@echo "  install-deps - Install SFML dependencies"
	@echo "  help	  - Show this help"

# Declare phony targets
.PHONY: all clean rebuild run debug release install-deps help