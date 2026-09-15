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
[[ -f bin/cit10a && -x bin/cit10a ]] || {
    # no build
    printf "%b\n" "$instl_n$err_msg cit10a build missing; build with ./build-cit10a.sh";
    printf "%b\n" "$instl_n$err_exit";
    exit 1;
}

if (( glob_instl )) ; then
    # /usr/local/bin/ installation
    printf "%b\n" "$instl_n$msg_msg sudo required for installation at $system_path";
    sudo mkdir -p "$install_dir";
    sudo install -m755 bin/cit10a "$install_dir/cit10a";
    printf "%b\n" "";
else
    # $HOME/.local/bin/ installation
    mkdir -p "$install_dir";
    install -m755 bin/cit10a "$install_dir/cit10a";
fi

# success log
printf "%b\n" "$instl_n$dbg_msg installed at $install_dir/cit10a \x1b[2m[ $(date +"%Y-%m-%d") ]\x1b[0m";
printf "%b\n" "$instl_n$msg_msg ./uninstall-cit10a.sh to uninstall";
exit 0;
