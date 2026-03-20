#!/usr/bin/env fish

set script_dir (dirname (status --current-filename))
source $script_dir/../../scripts/cxlkv_helpers.fish
set project_key thirdparty_libs/colorful_thread_safe_print

if cxlkv_clean_done $project_key
    exit 0
end

cxlkv_clean_local_outputs $script_dir
cxlkv_mark_clean_done $project_key
