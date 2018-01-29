#!/bin/bash
export LD_LIBRARY_PATH=.
export LD_INSERT_LIBRARIES="libft_malloc_x86_64_Linux.so"
export LD_FORCE_FLAT_NAMESPACE=1
export DYLD_LIBRARY_PATH=.
export DYLD_INSERT_LIBRARIES="libft_malloc_x86_64_Linux.so"
export DYLD_FORCE_FLAT_NAMESPACE=1
$@
