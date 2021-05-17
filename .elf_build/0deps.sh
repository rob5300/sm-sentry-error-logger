#!/usr/bin/bash

# TODO(Johnny): Prompt if either: (1) OS is not linux. (2) aptitude is not installed. Abort if user declines to continue.

# Grab our build dependenices

# sudo apt-get install -y g++-4.1 gcc-4.1 make git bzip2

sudo apt-get install -y make git bzip2                                             # General dependencies
sudo apt-get install -y libz libz-dev                                              # General dependencies
sudo apt-get install -y mono mono-devel python3 python3-pip                        # AMBuild tool
sudo apt-get install -y ia32-libs lib32z1 lib32z1-dev libc6-dev-i386 libc6-i386    # x86 support

# Ensure our vendor libs exist

git pull 2> /dev/null
git submodule update --init --recursive
