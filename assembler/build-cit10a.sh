#!/usr/bin/env bash
set -euo pipefail;

# get config
root="$(cd "$(dirname "$0")" && pwd)";
cd "$root";
[[ -f config.sh ]] || {
    # build script error
    printf "%b\n" "$root/config.sh missing";
    exit 1;
}
source config.sh;

# binary output
mkdir -p bin;

# flags / options
compiler="cc";
asserts=0;
sanitize=0;
cache_line=0;
cache_line_def=0;
native=0;
profile=0;
nthread=0;
while getopts ":hc:asl:mpn" opt ; do
    case $opt in
        h)
            build_flag_help;
            exit 0;
            ;;
        c)
            compiler="$OPTARG";
            ;;
        a)
            asserts=1;
            ;;
        s)
            sanitize=1;
            ;;
        l)
            cache_line_def=1;
            cache_line="$OPTARG";
            ;;
        m)
            native=1;
            ;;
        p)
            profile=1;
            ;;
        n)
            nthread=1;
            ;;
        \?)
            printf "%b\n" "$build_n$err_msg unknown build flag \`-$OPTARG\`";
            build_flag_help;
            printf "%b\n" "$build_n$err_exit";
            exit 1;
            ;;
        :)
            printf "%b\n" "$build_n$err_msg build flag \`-$OPTARG\` requires an argument";
            build_flag_help;
            printf "%b\n" "$build_n$err_exit";
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
);

if (( !nthread )) ; then
    # threading on
    flags+=( -pthread );
else
    # threading off
    flags+=( -DNTHREAD );
fi

if (( !asserts )) ; then
    # assertions on
    flags+=( -DNDEBUG );
fi

if (( sanitize )) ; then
    # sanitized build
    flags+=(
        -Og
        -g
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    );
else
    # optimized build
    flags+=(
        -O2
        -flto
        -DNSANITIZE
    );
fi

if (( cache_line_def )) ; then
    # cache line size
    flags+=( -DCACHE_LINE_SIZE=$cache_line );
fi

if (( native )) ; then
    # native arch
    flags+=( -march=native );
fi

if (( profile )) ; then
    # profiling build
    flags+=(
        -g
        -fno-omit-frame-pointer
    );
fi

# check compiler
if ! command -v "$compiler" >/dev/null 2>&1 ; then
    printf "%b\n" "$build_n$err_msg c compiler $compiler not found";
    printf "%b\n" "$build_n$err_exit";
    exit 1;
fi

# check source files
[[ -d inc && -d src ]] || {
    printf "%b\n" "$build_n$err_msg missing cit10a source code";
    printf "%b\n" "$build_n$err_exit";
    exit 1;
}

# inc/ files
inc_dirs="-Iinc -I.. $(find inc -mindepth 1 -type d -exec printf -- '-I%s ' {} +)";

# src/ files
src_fs="$(find src -type f -name '*.c' -exec printf '%s ' {} +)";
if [[ -z src_fs ]]; then
    printf "%b\n" "$build_n$err_msg missing cit10a src/ files\n";
    printf "%b\n" "$build_n$err_exit";
    exit 1;
fi

# compile compiler
$compiler "${flags[@]}" $inc_dirs $src_fs -o bin/cit10a || {
    printf "%b\n" "$build_n$err_msg $compiler compiler failure";
    printf "%b\n" "$build_n$err_exit";
    exit 1;
}

# success log
printf "%b\n" "$build_n$dbg_msg built at $root/bin/cit10a \x1b[2m[ $(date +"%Y-%m-%d") ]\x1b[0m";
exit 0;
