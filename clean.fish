#!/usr/bin/env fish

set script_dir (dirname (status --current-filename))
rm -rf $script_dir/build
rm -rf $script_dir/.cache
