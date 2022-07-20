#!/bin/bash
asciidoctor-multipage book.adoc -D build_html/
cp -r images build_html/images
