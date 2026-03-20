#!/usr/bin/env fish

set script_dir (realpath (dirname (status --current-filename)))

for path in \
    $script_dir/build \
    $script_dir/.cache \
    $script_dir/compile_commands.json
    if test -e $path
        echo "Removing $path"
        rm -rf $path
    end
end
