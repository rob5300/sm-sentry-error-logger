#!/usr/bin/bash

_BasePath=$(git rev-parse --show-toplevel)
_BaseDirName=$(git rev-parse --show-toplevel | grep -o --color=never '[A-Za-z_]*$')
_ExtDirName=$(echo $_BaseDirName | sed 's/ /_/g' | tr '[:upper:]' '[:lower:]') # 'CTF Repo' -> 'ctf_repo'

cd $_BasePath

_ExtPath="./vendor/sourcemod/public/extensions/$_ExtDirName"
if [[ ! -d $_ExtPath ]]; then
    # Make it so SM can build our extension
    ln -sr "." "$_ExtPath"
fi

pip install -q ./vendor/ambuild

# Create our build directory
if [[ -d "./build" ]]; then
    rm ./build -rf
fi
mkdir ./build
cd ./build

echo "$_BasePath/vendor/source-sdk-2013"
ls "$_BasePath/vendor/source-sdk-2013"
python ../configure.py --sdks default --hl2sdk-root "$_BasePath/vendor/source-sdk-2013" --mms-path "$_BasePath/vendor/metamod-source" --sm-path "$_BasePath/vendor/sourcemod"
