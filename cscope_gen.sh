#!/usr/bin/env bash

function join {
	local separator="$1"
	shift
	local retval="$( printf "${separator}%s" "$@" )"
	retval="${retval:${#separator}}"
	echo "${retval}"
}

excl_directories=(
    ./action_skel_code
    ./.git
    ./lib
    "*/gtest"
)
excl_paths="-path $(join " -prune -o -path " "${excl_directories[@]}") -prune"

excl_wholefiles=(
)
if [ ${#excl_wholefiles[@]} -gt 0 ]; then
    excl_files="-a ! -wholename $(join " -a ! -wholename " "${excl_wholefiles[@]}")"
else
    excl_files=""
fi

include_extensions=( cpp inl hpp h c )
incl_extensions="-name *.$(join " -o -name *." "${include_extensions[@]}")"

#Don't expand * - see http://stackoverflow.com/questions/102049/how-do-i-escape-the-wildcard-asterisk-character-in-bash
set -f
find . \( $excl_paths -o \( $incl_extensions \) \) -a -type f $excl_files > cscope.files
set +f

exec cscope -b -q
