#!/usr/bin/env bash
set -euo pipefail;

# colors / strings
build_n="\x1b[0m\x1b[4m\x1b[3macama-build\x1b[0m -";
instl_n="\x1b[0m\x1b[4m\x1b[3macama-install\x1b[0m -";
unins_n="\x1b[0m\x1b[4m\x1b[3macama-uninstall\x1b[0m -";

dbg_clr="\x1b[36m";
msg_clr="\x1b[34m";
wrn_clr="\x1b[33m";
err_clr="\x1b[31m";
scs_clr="\x1b[32m";

dbg_msg=" \x1b[36minfo\x1b[0m :";
msg_msg=" \x1b[34mnote\x1b[0m :";
wrn_msg=" \x1b[33mwarning\x1b[0m :";
err_msg=" \x1b[31merror\x1b[0m :";

# exit strings
err_exit="$err_clr exit code 1\x1b[0m";
scs_exit="$scs_clr exit code 0\x1b[0m";

# install paths
user_path="$HOME/.local/bin";
system_path="/usr/local/bin";

# flag help information
build_flag_help() {
    printf "%b\n" "$build_n$msg_msg build flags";
    printf "%b\n" "    \x1b[1m-h\x1b[0m                - flag usage help";
    printf "%b\n" "    \x1b[1m-c \x1b[2m<compiler>\x1b[0m     - manually set c compiler - defaults to cc";
    printf "%b\n" "    \x1b[1m-a\x1b[0m                - code assertions enabled (helps catch some compiler bugs)";
    printf "%b\n" "    \x1b[1m-s\x1b[0m                - sanitizes build (recommended if -a doesn't work)";
    printf "%b\n" "    \x1b[1m-l \x1b[2m<line size>\x1b[0m    - manually set cache line size (must be 2^n)";
    printf "%b\n" "    \x1b[1m-n\x1b[0m                - compile for native machine arch (-march=native)";
    printf "%b\n" "    \x1b[1m-p\x1b[0m                - compile for profiling build";
}
install_flag_help() {
    printf "%b\n" "$instl_n$msg_msg install flags";
    printf "%b\n" "    \x1b[1m-h\x1b[0m                - flag usage help";
    printf "%b\n" "    \x1b[1m-g\x1b[0m                - install at $system_path instead of $user_path";
}
uninstall_flag_help() {
    printf "%b\n" "$unins_n$msg_msg uninstall flags";
    printf "%b\n" "    uninstalling takes no arguments";
}
