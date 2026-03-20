#!/usr/bin/env fish

set script_dir (dirname (status --current-filename))
source $script_dir/../../scripts/cxlkv_helpers.fish
set install_script $script_dir/install_deps.sh
set project_key thirdparty_libs/colorful_thread_safe_print

cxlkv_prepare_build_env $script_dir

if cxlkv_build_done $project_key
    exit 0
end

if test -x $install_script
    bash $install_script
    or exit $status
else
    echo "Missing dependency installer: $install_script" >&2
    exit 1
end

cxlkv_configure_and_build $script_dir
or exit $status

cxlkv_mark_build_done $project_key
