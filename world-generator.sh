#!/bin/bash

set -e

# Clone the repository into /var/lib/zerotier-one
cd /var/lib/zerotier-one
apt-get update & apt-get install git -y
git clone https://github.com/imashen/zerotier-world-generator.git

# Enter the project directory
cd zerotier-world-generator

# Run the do.sh script
./do.sh