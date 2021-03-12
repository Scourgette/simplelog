#!/bin/bash -e
doxygen ./docs/doxygen/simplelog.dox
make -C ./docs/sphinx html
