#!/bin/bash


export BOOTLOADER=1
export CODEC=1



export TARGET=`basename $0 .sh | sed s/_all//g`

./${TARGET}.sh

