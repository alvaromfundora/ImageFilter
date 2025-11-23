# Makefile for Image Quality Enhancement Program
# This automates the compilation of all source files

# Compiler
CXX = g++

# Compiler flags
# -Wall: Enable all warnings
# -std=c++11: Use C++11 standard
# -I/usr/include/opencv4: Include OpenCV headers
CXXFLAGS = -Wall -std=c++11 -I/usr/include/opencv4

# Linker flags
# Link OpenCV libraries needed for the program
LDFLAGS = -lopencv_core -lopencv_imgcodecs -lopencv_imgproc

# Output executable name
TARGET = image_enhancer

# Source files
SOURCES = main.cpp psnr.cpp ssim.cpp filters.cpp

# Object files (automatically generated from source files)
OBJECTS = $(SOURCES:.cpp=.o)

# Default target - builds the executable
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete! Executable: $(TARGET)"

# Compile source files to object files
%.o: %.cpp image_quality.h
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up compiled files
clean:
	@echo "Cleaning up..."
	rm -f $(OBJECTS) $(TARGET) output_*.jpg
	@echo "Clean complete!"

# Remove only output images
clean-output:
	@echo "Removing output images..."
	rm -f output_*.jpg
	@echo "Output images removed!"

# Rebuild everything from scratch
rebuild: clean all

# Run the program with a test image
run:
	./$(TARGET) input_image.jpg

# Help target - shows available commands
help:
	@echo "Available targets:"
	@echo "  make          - Build the program"
	@echo "  make clean    - Remove compiled files and outputs"
	@echo "  make rebuild  - Clean and rebuild from scratch"
	@echo "  make run      - Build and run with input_image.jpg"
	@echo "  make help     - Show this help message"

# Mark phony targets (targets that don't create files)
.PHONY: all clean clean-output rebuild run help