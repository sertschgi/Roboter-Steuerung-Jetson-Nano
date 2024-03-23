#!/bin/bash

mkdir -p /etc/OpenCL/vendors/
cd /etc/OpenCL/vendors/

touch pocl.icd

echo "/usr/local/pocl/lib/libpocl.so" >> pocl.icd

echo "Info: Installed pocl"
