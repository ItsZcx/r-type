# Default target
.PHONY: all
all: install build

# Executable name
EXECUTABLE = server

# Build type (default to Release)
BUILD_TYPE ?= Release

# Build directory
BUILD_DIR = build

# Get SDK path
SDK_PATH := $(shell xcrun --show-sdk-path)

# Conan install
.PHONY: install
install:
	@echo "Installing Conan dependencies..."
	@conan install . --build missing -s build_type=$(BUILD_TYPE) -c tools.apple:sdk_path=$(SDK_PATH) -of $(BUILD_DIR)

# Build target
.PHONY: build
build:
	@echo "Building project..."
	@if [ ! -d $(BUILD_DIR) ]; then \
		mkdir $(BUILD_DIR); \
	fi

	@cd $(BUILD_DIR) && cmake -DBUILD_IN_DOCKER=ON .. -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake && make

# Clean target
.PHONY: clean
clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)

# Rebuild target
.PHONY: rebuild
rebuild: clean build

# Help target
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  make          - Build the project"
	@echo "  make install    - Install Conan dependencies"
	@echo "  make clean    - Remove the build directory"
	@echo "  make rebuild  - Clean and rebuild the project"
	@echo "  make help     - Display this help message"
