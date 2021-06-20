install -m 00755 -D   bazel-bin/unix/ibus/ibus_mozc     /usr/lib/ibus-mozc/ibus-engine-mozc
install -m 00755 -D   bazel-bin/renderer/mozc_renderer  /usr/lib/mozc/mozc_renderer
install -m 00755      bazel-bin/server/mozc_server      /usr/lib/mozc/mozc_server
install -m 00755      bazel-bin/gui/tool/mozc_tool      /usr/lib/mozc/mozc_tool
# install -m 00755      bazel-bin/unix/emacs/mozc_emacs_helper /usr/lib/mozc/mozc_emacs_helper
# bazel-bin/unix/icons.zip => not necessary for frequent small updates




