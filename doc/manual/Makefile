all: dvi html

dvi: manual.dvi

manual.dvi: manual.tex
	latex manual.tex

html: manual/index.html

manual/index.html: manual.tex
	latex2html manual.tex -local_icons

pdf: manual.pdf

manual.pdf: manual.tex
	pdflatex manual.tex

clean:
	rm -f manual.aux manual.dvi manual.log manual.out manual.pdf manual.toc
	rm -rf manual
