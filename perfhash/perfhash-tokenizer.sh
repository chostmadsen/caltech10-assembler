#!/usr/bin/env bash
set -euo pipefail;

# go to root
root="$(cd "$(dirname "$0")" && pwd)";
cd "$root";
assembler_dir="$(cd "$root/../assembler" && pwd)";

# cleanup catch
cleanup() { rm -f "$assembler_dir/perfhash.out"; }
trap cleanup EXIT INT TERM;

# argparse
debug_build=0;
nthread=0;
cache_line=0;
cache_line_def=0;
compiler="cc";
while getopts ":dnhc:l:" opt ; do
    case $opt in
        d)
            debug_build=1;
            ;;
        n)
            nthread=1;
            ;;
        c)
            compiler="$OPTARG";
            ;;
        l)
            cache_line_def=1;
            cache_line="$OPTARG";
            ;;
        h)
            printf "%b\n" "-d for debugging\n"
                          "-n to disable multithreading\n"
                          "-c [compiler] to specify a c compiler\n"
                          "-l [line size] to specify cache line size";
            exit 0;
            ;;
        \?)
            printf "%b\n" "perfhash unknown build flag";
            exit 1;
            ;;
    esac
done

# default flags
flags=(
    -std=c23
    -Wall
    -Wextra
    -Wpedantic
    -march=native
    -flto
    -funroll-loops
);

# flag processing
if (( debug_build )) ; then
    flags+=(
        -Og
        -g
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    );
else
    flags+=( -O3 );
fi

# threading toggle
if (( !nthread )) ; then
    flags+=( -pthread );
else
    flags+=( -DNTHREAD );
fi

if (( cache_line_def )) ; then
    # cache line size
    flags+=( -DCACHE_LINE_SIZE=$cache_line );
fi

# check compiler
if ! command -v "$compiler" >/dev/null 2>&1 ; then
    printf "%b\n" "c compiler $compiler not found";
    exit 1;
fi

# compilation
$compiler ${flags[@]} -Iinc -I.. -I../assembler/inc src/*.c impl/assembler_main.c -o "$assembler_dir/perfhash.out";

# move to correct dir and run
cd "$assembler_dir";
./perfhash.out;

# clean and exit
cleanup;
exit 0;
