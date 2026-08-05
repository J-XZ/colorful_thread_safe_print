#!/usr/bin/env fish

set script_dir (dirname (realpath (status filename)))
source $script_dir/../../scripts/cxlkv_helpers.fish

# 变体隔离：构建目录绑定 build type、ASAN、frame-pointer、malloc 和
# LATENCY_SIM_COMPILE_OFF（由公共 helper 的 cxlkv_build_dir 统一计算），不再写入
# 单一 thirdparty_libs/colorful_thread_safe_print/build/，避免不同变体互相覆盖。
# install_deps.sh 是唯一依赖脚本，cxlkv_build_project 会在检查父 stamp 前先执行它。
cxlkv_build_project \
    thirdparty_libs/colorful_thread_safe_print \
    $script_dir \
    $script_dir/install_deps.sh
or exit $status
