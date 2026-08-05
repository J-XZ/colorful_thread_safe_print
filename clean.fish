#!/usr/bin/env fish

set script_dir (dirname (realpath (status filename)))
source $script_dir/../../scripts/cxlkv_helpers.fish

cxlkv_clean_project \
    thirdparty_libs/colorful_thread_safe_print \
    $script_dir
or exit $status
