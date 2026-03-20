#!/usr/bin/env fish

set script_dir (dirname (status --current-filename))
set build_dir $script_dir/build
set test_binary $build_dir/mytest0
set built_library

for candidate in \
    $build_dir/libcolorful_thread_safe_print.a
    if test -e $candidate
        set built_library $candidate
        break
    end
end

if not test -x $test_binary; or test -z "$built_library"
    echo "Build artifacts are missing. Building project first..."
    fish $script_dir/build.fish
end

if test -x $test_binary
    ctest --test-dir $build_dir --output-on-failure
else
    echo "Missing test binary: $test_binary" >&2
    exit 1
end
