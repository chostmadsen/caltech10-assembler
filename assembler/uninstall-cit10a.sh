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

while getopts ":h" opt ; do
    case $opt in
        h)
            uninstall_flag_help;
            exit 0;
            ;;
        \?)
            printf "%b\n" "$unins_n$err_msg unknown build flag \`-$OPTARG\`";
            uninstall_flag_help;
            printf "%b\n" "$unins_n$err_exit";
            exit 1;
            ;;
    esac
done

# uninstall flag
has_install=0;

# remove installations
if [[ -f "$user_path/cit10a" ]] ; then
    rm -f "$user_path/cit10a";
    has_install=1;
fi
if [[ -f "$system_path/cit10a" ]] ; then
    printf "%b\n" "$unins_n$msg_msg sudo required for cit10a global uninstallation";
    sudo rm -f "$system_path/cit10a";
    has_install=1;
    printf "%b\n" "";
fi

# success log
if (( has_install )) ; then
    printf "%b\n" "$unins_n$dbg_msg uninstalled \x1b[2m[ $(date +"%Y-%m-%d") ]\x1b[0m";
else
    printf "%b\n" "$unins_n$wrn_msg no cit10a installation";
fi
exit 0;
