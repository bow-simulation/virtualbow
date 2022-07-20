# VirtualBow User Manual

The VirtualBow user manual is created with [Asciidoctor](https://asciidoctor.org/), a tool that reads files in AsciiDoc format and converts them into various output formats.
Asciidoctor and the needed extensions for [generating PDF](https://asciidoctor.org/docs/asciidoctor-pdf/), [multi-page HTML](https://github.com/owenh000/asciidoctor-multipage), [syntax highlighting](https://docs.asciidoctor.org/asciidoctor/latest/syntax-highlighting/pygments/) and [math expressions](https://github.com/asciidoctor/asciidoctor-mathematical) require [Ruby](https://www.ruby-lang.org/) are installed with

    gem install asciidoctor
    gem install asciidoctor-pdf
    gem install asciidoctor-multipage
    gem install asciidoctor-mathematical
    gem install pygments.rb

For `asciidoctor-mathematical`, see [the readme](https://github.com/asciidoctor/asciidoctor-mathematical) for additional dependencies.
    
Run the files `source/build_html.sh` and `source/build_pdf.sh` for generating the respective output.
