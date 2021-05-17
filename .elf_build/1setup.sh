#!/usr/bin/bash

_BasePath=$(git rev-parse --show-toplevel)
_BaseDirName=$(git rev-parse --show-toplevel | grep -o --color=never '[A-Za-z_]*$')
_ExtDirName=$(echo $_BasePathRoot | sed 's/ /_/g' | tr '[:upper:]' '[:lower:]') # 'CTF Repo' -> 'ctf_repo'
#_ExtPath="./vendor/sourcemod/public/extensions/$_ExtDirName"

# if [[ ! -d $_ExtPath ]]; then
#     # Make it so SM can build our extension
#     ln -sr "./" "$_ExtPath"
# fi

cd _BaseDirName

