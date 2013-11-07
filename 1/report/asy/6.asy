import graph3;
import palette;
size(225pt,0,IgnoreAspect);
currentprojection=orthographic(1,-1.8,1);
real X=4.5; real M=abs(gamma((X,0)));
pair Gamma(pair z) {
return (z.x > 0 || z != floor(z.x)) ?
gamma(z) : M;
}
real f(pair z) {return min(abs(Gamma(z)),M);}
surface s=surface(f,(-2.1,-2),(X,2),60,Spline);
real Arg(triple v) {
return degrees(Gamma((v.x,v.y)),warn=false);
}
s.colors(palette(s.map(Arg),Wheel()));
draw(s);