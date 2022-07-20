#!/bin/bash
asciidoctor-pdf book.adoc -r asciidoctor-mathematical -a mathematical-format=svg -D build_pdf/
