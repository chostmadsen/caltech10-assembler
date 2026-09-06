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

# install flags
glob_instl=0;
install_dir="$user_path";
while getopts ":hg" opt ; do
    case $opt in
        h)
            install_flag_help;
            printf "%b\n" "$instl_n$scs_exit";
            exit 0;
            ;;
        g)
            glob_instl=1;
            install_dir="$system_path";
            ;;
        \?)
            printf "%b\n" "$instl_n$err_msg unknown build flag \`-$OPTARG\`";
            install_flag_help;
            printf "%b\n" "$instl_n$err_exit";
            exit 1;
            ;;
    esac
done

# get build
[[ -f bin/acama && -x bin/acama ]] || {
    # no build
    printf "%b\n" "$instl_n$err_msg acama build missing; build with ./build-acama.sh";
    printf "%b\n" "$instl_n$err_exit";
    exit 1;
}

if (( glob_instl )) ; then
    # /usr/local/bin/ installation
    printf "%b\n" "$instl_n$msg_msg sudo required for installation at $system_path";
    sudo mkdir -p "$install_dir";
    sudo install -m755 bin/acama "$install_dir/acama";
    printf "%b\n" "";
else
    # $HOME/.local/bin/ installation
    mkdir -p "$install_dir";
    install -m755 bin/acama "$install_dir/acama";
fi

# success log
printf "%b\n" "$instl_n$dbg_msg installed at $install_dir/acama \x1b[2m[ $(date +"%Y-%m-%d") ]\x1b[0m";
printf "%b\n" "$instl_n$msg_msg ./uninstall-acama.sh to uninstall";
printf "%b\n" "$instl_n$scs_exit";
exit 0;
