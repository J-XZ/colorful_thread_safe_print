#!/usr/bin/env fish

if not set -q CMAKE_BUILD_TYPE
    set CMAKE_BUILD_TYPE Debug
end

set script_dir (dirname (status --current-filename))
set install_script $script_dir/install_deps.sh

if test -x $install_script
    bash $install_script
else
    echo "Missing dependency installer: $install_script" >&2
    exit 1
end

mkdir -p $script_dir/build
cd $script_dir/build

cmake -G Ninja -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE ..

ninja
