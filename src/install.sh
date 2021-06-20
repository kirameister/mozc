pushd out_linux/Release
install -m 00755 -D ibus_mozc      /usr/lib/ibus-mozc/ibus-engine-mozc
install -m 00755 -D mozc_renderer  /usr/lib/mozc/mozc_renderer
install -m 00755    mozc_server    /usr/lib/mozc/mozc_server
install -m 00755    mozc_tool      /usr/lib/mozc/mozc_tool
install -m 00644 -D gen/unix/ibus/mozc.xml  /usr/share/ibus/component/mozc.xml
popd
pushd data/images/unix
install -m 00644 -D ui-alpha_full.png  /usr/share/ibus-mozc/alpha_full.png
install -m 00644    ui-alpha_half.png  /usr/share/ibus-mozc/alpha_half.png
install -m 00644    ui-dictionary.png  /usr/share/ibus-mozc/dictionary.png
install -m 00644    ui-direct.png      /usr/share/ibus-mozc/direct.png
install -m 00644    ui-hiragana.png    /usr/share/ibus-mozc/hiragana.png
install -m 00644    ui-katakana_full.png  /usr/share/ibus-mozc/katakana_full.png
install -m 00644    ui-katakana_half.png  /usr/share/ibus-mozc/katakana_half.png
install -m 00644    ime_product_icon_opensource-32.png  /usr/share/ibus-mozc/product_icon.png
install -m 00644    ui-properties.png  /usr/share/ibus-mozc/properties.png
install -m 00644    ui-tool.png  /usr/share/ibus-mozc/tool.png
popd
