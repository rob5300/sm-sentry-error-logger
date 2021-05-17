#!/usr/bin/bash

_BasePath=$(git rev-parse --show-toplevel)
_BaseDirName=$(git rev-parse --show-toplevel | grep -o --color=never '[A-Za-z_]*$')
_ExtDirName=$(echo $_BasePath | sed 's/ /_/g' | tr '[:upper:]' '[:lower:]') # 'CTF Repo' -> 'ctf_repo'


cd $_BasePath

_ExtPath="./vendor/sourcemod/public/extensions/$_ExtDirName"
if [[ ! -d $_ExtPath ]]; then
    # Make it so SM can build our extension
    ln -sr "." "$_ExtPath"
fi


pip install ./vendor/ambuild
python3 ./configure.py --sdks default --hl2sdk-root ./vendor/source-sdk-2013 --mms-path ./vendor/metamod-source --sm-path ./vendor/sourcemod
