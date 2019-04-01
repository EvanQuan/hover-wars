#!/bin/bash

# Pull from hover-wars-fmod submodule
# Only necessary if working with FMOD.
# The main project compiles and runs fine without the submodule.

git submodule update --recursive --remote
