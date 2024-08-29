#!/bin/bash

# Define the output directory
OUTPUT_DIR="bin"

# Create the output directory if it does not exist
mkdir -p "$OUTPUT_DIR"

# Build mkplanet
c++ -std=c++11 -I../.. -I../../ext -I.. -g -o mkplanet ../../node/C25519.cpp ../../node/Salsa20.cpp ../../node/SHA512.cpp ../../node/Identity.cpp ../../node/Utils.cpp ../../node/InetAddress.cpp ../../osdep/OSUtils.cpp mkplanet.cpp -lm
if [ $? -ne 0 ]; then
    echo "Error: Failed to build mkplanet"
    exit 1
fi


# Move the binaries to the output directory
mv mkplanet "$OUTPUT_DIR/"
if [ $? -ne 0 ]; then
    echo "Error: Failed to move binaries to $OUTPUT_DIR"
    exit 1
fi

echo "Build completed successfully."