// échelle
unitsize(8mm);
// notre fonction
import graph;
real f(real x) {return x*cos(x)-sin(x);}
path p=graph(f,-2*pi-.5,2*pi+.5,operator ..);
// les axes
draw((-2*pi-.5,0)--(2*pi+1,0),Arrow(HookHead));
draw((0,0)--(1,0),Arrow(HookHead));
draw((0,-2*pi-.5)--(0,2*pi+1),Arrow(HookHead));
draw((0,0)--(0,1),Arrow(HookHead));
// repérage sur les axes
draw((-2*pi,.1)--(-2*pi,-.1));label("$-2\pi$",(-2*pi,-.1),S);
draw((-pi,.1)--(-pi,-.1));label("$-\pi$",(-pi,-.1),S);
draw((2*pi,.1)--(2*pi,-.1));label("$2\pi$",(2*pi,-.1),S);
draw((pi,.1)--(pi,-.1));label("$\pi$",(pi,-.1),S);
draw((-.1,-2*pi)--(.1,-2*pi));label("$-2\pi$",(.1,-2*pi),E);
draw((-.1,-pi)--(.1,-pi));label("$-\pi$",(.1,-pi),E);
draw((-.1,2*pi)--(.1,2*pi));label("$2\pi$",(.1,2*pi),E);
draw((-.1,pi)--(.1,pi));label("$\pi$",(.1,pi),E);
// les tangentes
draw((-2*pi-.7,-2*pi)--(-2*pi+.7,-2*pi),blue,Arrows(SimpleHead));
draw((-pi-.7,pi)--(-pi+.7,pi),blue,Arrows(SimpleHead));
draw((2*pi-.7,2*pi)--(2*pi+.7,2*pi),blue,Arrows(SimpleHead));
draw((pi-.7,-pi)--(pi+.7,-pi),blue,Arrows(SimpleHead));
draw((-.7,0)--(.7,0),blue,Arrows(SimpleHead));
// la courbe
draw(p,red);