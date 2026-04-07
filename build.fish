#!/usr/bin/env fish

set script_dir (realpath (dirname (status --current-filename)))
set install_script $script_dir/install_deps.sh

function compute_build_jobs
    set -l cpu_cores 1
    if command -sq nproc
        set cpu_cores (nproc)
    end
    if test -z "$cpu_cores"
        set cpu_cores 1
    end

    set -l mem_available_kb 0
    if test -r /proc/meminfo
        set mem_available_kb (awk '/^MemAvailable:/ {print $2; exit}' /proc/meminfo)
    end
    if test -z "$mem_available_kb"
        set mem_available_kb 0
    end

    # 1.5GB ~= 1572864 KB
    set -l mem_jobs (math -s0 "$mem_available_kb / 1572864")
    if test -z "$mem_jobs"
        set mem_jobs 1
    end
    if test $mem_jobs -lt 1
        set mem_jobs 1
    end

    set -l jobs $cpu_cores
    if test $mem_jobs -lt $jobs
        set jobs $mem_jobs
    end
    if test $jobs -lt 1
        set jobs 1
    end
    echo $jobs
end

if not set -q CMAKE_BUILD_TYPE
    set -gx CMAKE_BUILD_TYPE Debug
else
    set -gx CMAKE_BUILD_TYPE $CMAKE_BUILD_TYPE
end
if not set -q CTSP_ENABLE_ASAN
    if set -q CXLKV_ENABLE_ASAN
        set -gx CTSP_ENABLE_ASAN (string upper -- $CXLKV_ENABLE_ASAN)
    else
        set -gx CTSP_ENABLE_ASAN OFF
    end
else
    set -gx CTSP_ENABLE_ASAN (string upper -- $CTSP_ENABLE_ASAN)
end
if not set -q CTSP_BUILD_JOBS
    if set -q CXLKV_BUILD_JOBS
        set -gx CTSP_BUILD_JOBS $CXLKV_BUILD_JOBS
    else if set -q CMAKE_BUILD_PARALLEL_LEVEL
        set -gx CTSP_BUILD_JOBS $CMAKE_BUILD_PARALLEL_LEVEL
    else
        set -gx CTSP_BUILD_JOBS (compute_build_jobs)
    end
end
set -gx CMAKE_BUILD_PARALLEL_LEVEL $CTSP_BUILD_JOBS

set build_dir $script_dir/build
set cache_dir $script_dir/.cache
set tmp_dir $cache_dir/tmp
set ccache_dir $cache_dir/ccache
if set -q CCACHE_DIR
    set ccache_dir $CCACHE_DIR
end

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
cmake -G Ninja \
    -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE \
    -DCTSP_ENABLE_ASAN=$CTSP_ENABLE_ASAN \
    ..
or begin
    popd >/dev/null
    exit 1
end

cmake --build . --parallel $CTSP_BUILD_JOBS
set build_status $status
popd >/dev/null

for path in $cache_dir/tmp $cache_dir
    if test -d $path
        rmdir $path 2>/dev/null
    end
end

exit $build_status
