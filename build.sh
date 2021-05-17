#!/usr/bin/bash

_SavedCwd=$(pwd)

./.elf_build/0deps.sh
./.elf_build/1setup.sh
./.elf_build/2build.sh

cd $_SavedCwd
