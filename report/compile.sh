python texbuilder.py tex test.tex\
&& python stybuilder.py sty perso.sty\
&& xelatex --shell-escape test.tex\
&& makeindex test\
&& bibtex test\
&& xelatex --shell-escape test.tex\
&& xelatex --shell-escape test.tex