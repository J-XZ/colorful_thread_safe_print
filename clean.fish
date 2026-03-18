#!/usr/bin/env fish

set script_dir (dirname (status --current-filename))
set paths_to_remove \
    $script_dir/build \
    $script_dir/.cache

for path in $paths_to_remove
    if test -e $path
        echo "Removing $path"
        rm -rf $path
    else
        echo "Skipping missing path: $path"
    end
end
