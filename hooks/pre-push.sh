#!/bin/sh

# Build directories
CLIENT_BUILD_DIR="sources/client/build"
SERVER_BUILD_DIR="sources/server/build"

# Create build directories if they don't exist
mkdir -p $CLIENT_BUILD_DIR
mkdir -p $SERVER_BUILD_DIR

# Build client
cd $CLIENT_BUILD_DIR
cmake ..
cmake --build .
cd ../../../

# Build server
cd $SERVER_BUILD_DIR
cmake ..
cmake --build .
cd ../../../

# Ensure Node.js and npm are installed (for Debian/Ubuntu-based systems)
if ! command -v node > /dev/null 2>&1; then
    echo "Node.js is not installed. Installing Node.js and npm..."
    curl -fsSL https://deb.nodesource.com/setup_18.x | bash -
    apt-get install -y nodejs
fi

# Deploy the unified doxygen documentation
CLIENT_DOCS_DIR="sources/client/build/doxygen/html"
SERVER_DOCS_DIR="sources/server/build/doxygen/html"
npx vercel deploy --prod $CLIENT_DOCS_DIR
npx vercel deploy --prod $SERVER_DOCS_DIR
