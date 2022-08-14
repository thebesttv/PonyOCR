# PonyOCR

![](/img/PonyOCR.png)

A simple tool for fetching OCR results from Baidu, Tencent, Mathpix, etc.

## Supported OCR Mode:
* Normal Text
* Table
  |name|price|
  |----|-----|
  |apple| $10$ |
  |pony | $\infty$ |
* Mathematical Formula (KaTeX)
  \\[ f(pony) = \int_{-\infty}^{\infty} \text{doubly fun}\\]
* Handwriting
* Web Image (optimised for images with complex background and WordArt)
* Plain number
* even QR Code


## Built with

* Qt 5.14 (for now)

## Files

There are some config files under directory `files/`:
* `org.thebesttv.PonyOCR.service`: should be under
  `/usr/share/dbus-1/services/`, allows dbus to auto start PonyOCR when
  `PonyOCR request` command is issued
* `PonyOCR.desktop`: should be under `~/.local/share/applications/`,
  adds PonyOCR icon to desktop.  The icon file is
  `/usr/share/icons/hicolor/48x48/apps/ponyocr.png`.
  See [Desktop files: putting your application in the desktop menus](https://developer.gnome.org/integration-guide/stable/desktop-files.html.en).


The config file is `~/.config/thebesttv/PonyOCR.conf`.

## Acknowledgment

* flameshot
* Qt WebEngine Markdown Editor Example
* XLor
