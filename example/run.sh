#!/bin/sh

my_rpath='../'

env LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${my_rpath} ./$*
