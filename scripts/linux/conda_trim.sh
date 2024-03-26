#!/bin/bash
set -e

BASE_PREFIX=$(conda info --base)
if [[ "$CONDA_PREFIX" == "$BASE_PREFIX" ]]; then
    echo "Please run this script inside a non-base conda environment."
    exit 1
fi

## In our .env we ship a lot of stuff that we do not strictly need
## Let's remove that. 

## Saves 1gb - Go is handled by gradle. 
conda remove go --offline -y
## Also saves 1gb
conda remove rust-std-aarch64-apple-darwin rust-std-x86_64-apple-darwin rust-std-aarch64-apple-ios rust-std-x86_64-apple-ios --offline -y
## Clean the PKG cache
conda clean -a -y 
