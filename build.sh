#!/usr/bin/bash

_SavedCwd=$(pwd)
_BasePath=$(git rev-parse --show-toplevel)
_BaseDirName=$(git rev-parse --show-toplevel | grep -o --color=never '[A-Za-z_]*$')


# | PRE-CONDITION CHECKS
# =====================
# TODO(Johnny): Prompt if either: (1) OS is not linux. (2) aptitude is not installed. Abort if user declines to continue.
# Grab our build dependenices
# sudo apt-get install -y g++-4.1 gcc-4.1 make git bzip2
sudo apt-get install -y make git bzip2                                             # General dependencies
sudo apt-get install -y libz libz-dev                                              # General dependencies
sudo apt-get install -y mono mono-devel                                            # AMBuild tool
sudo apt-get install -y ia32-libs lib32z1 lib32z1-dev libc6-dev-i386 libc6-i386    # x86 support
# Ensure our vendor libs exist
git pull 2> /dev/null
git submodule update --init --recursive

wget http://ftp.gnu.org/gnu/gcc/gcc-4.1.2/gcc-4.1.2.tar.bz2
bzip2 -d gcc-4.1.2.tar.bz2
tar xf gcc-4.1.2.tar.bz2
rm gcc-4.1.2.tar


# | SETUP PIPELINE
# =====================
_ExtDirName=$(echo $_BasePathRoot | sed 's/ /_/g' | tr '[:upper:]' '[:lower:]') # 'CTF Repo' -> 'ctf_repo'
_ExtPath="./vendor/sourcemod/public/extensions/$_ExtDirName"
if [[ ! -d $_ExtPath ]]; then
    # Make it so SM can build our extension
    ln -sr "./" "$_ExtPath"
fi
# Make sure we're at the root of the repo.
cd $_BasePath


# | BUILD PIPELINE
# =====================
# Build the project
make original

# | CLEANUP PIPELINE
# =====================
# Navigate back to the user's last pwd
cd $_SavedCwd
