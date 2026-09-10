#!/usr/bin/env bash
set -euo pipefail;

root="$(cd "$(dirname "$0")" && pwd)";
cd "$root";

# args
cache_line_def=0;
cache_line=0;
while getopts ":hl:" opt ; do
    case $opt in
        h)
            printf "%s\n" "-l to set cache line size";
            exit 0;
            ;;
        l)
            cache_line_def=1;
            cache_line="$OPTARG";
            ;;
        \?)
            printf "%s\n" "unknown argument \`$OPTARG\`";
            exit 1;
            ;;
        :)
            printf "%s\n" "\`$OPTARG\` requires an argument";
            exit 1;
            ;;
    esac
done

# flags
flags=(
    -std=c23
    -Wall
    -Wextra
    -Wpedantic
);

if (( cache_line_def )) ; then
    # cache line size
    flags+=( -DCACHE_LINE_SIZE=$cache_line );
fi

cc "${flags[@]}" alignment_def.c -o alignment_def.out;
./alignment_def.out && rm alignment_def.out;
