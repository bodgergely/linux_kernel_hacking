#!/bin/bash
set -e
ROOT="$HACKING/kernel_hacking"
MODULE_DEV="$ROOT/module_dev"
if [[ -z $1 ]]
then
    echo Need to provide the module directory name as \$1 arg!
    exit -1
fi
MODULE=$1
cd $MODULE_DEV/$MODULE
make
cd $MODULE_DEV
$MODULE_DEV/create_initramfs-update.sh $MODULE
