#!/usr/bin/env fish

set script_dir (realpath (dirname (status --current-filename)))
set install_script $script_dir/install_deps.sh

if not set -q CMAKE_BUILD_TYPE
    set -gx CMAKE_BUILD_TYPE Debug
else
    set -gx CMAKE_BUILD_TYPE $CMAKE_BUILD_TYPE
end

set build_dir $script_dir/build
set cache_dir $script_dir/.cache
set tmp_dir $cache_dir/tmp
set ccache_dir $build_dir/ccache

mkdir -p $build_dir $cache_dir $tmp_dir $ccache_dir

set -gx TMPDIR $tmp_dir
set -gx TMP $tmp_dir
set -gx TEMP $tmp_dir
set -e XDG_RUNTIME_DIR
set -gx CCACHE_TEMPDIR $tmp_dir
set -gx CCACHE_DIR $ccache_dir

if test -x $install_script
    bash $install_script
    or exit $status
else
    echo "Missing dependency installer: $install_script" >&2
    exit 1
end

pushd $build_dir >/dev/null
cmake -G Ninja -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE ..
or begin
    popd >/dev/null
    exit 1
end

cmake --build .
set build_status $status
popd >/dev/null

for path in $build_dir/ccache $cache_dir/tmp $cache_dir
    if test -d $path
        rmdir $path 2>/dev/null
    end
end

exit $build_status
