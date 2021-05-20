#!/bin/bash

_BasePath=$(git rev-parse --show-toplevel)
_BaseDirName=$(git rev-parse --show-toplevel | grep -o --color=never '[A-Za-z_]*$')
_ExtDirName=$(echo $_BaseDirName | sed 's/ /_/g' | tr '[:upper:]' '[:lower:]') # 'CTF Repo' -> 'ctf_repo'

echo "cd '$_BasePath'"
cd $_BasePath

_ExtPath="./vendor/sourcemod/public/extensions/$_ExtDirName"
if [[ ! -d $_ExtPath ]]; then
    # Make it so SM can build our extension
    echo "ln -sr '.' '$_ExtPath'"
    ln -sr "." "$_ExtPath"
fi

echo "pip install -q ./vendor/ambuild"
pip install -q ./vendor/ambuild

echo "mkdir ./build"
if [[ -d "./build" ]]; then
    rm ./build -rf
fi
mkdir ./build
cd ./build

echo "python ../configure.py --sdks tf2 --hl2sdk-root '$_BasePath/vendor' --mms-path '$_BasePath/vendor' --sm-path '$_BasePath/vendor/sourcemod'"
python ../configure.py --sdks tf2 --hl2sdk-root "$_BasePath/vendor" --mms-path "$_BasePath/vendor" --sm-path "$_BasePath/vendor/sourcemod"

