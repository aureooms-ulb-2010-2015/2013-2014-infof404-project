import graph3;
import grid3;
import palette;
currentprojection=orthographic(0.8,0.7,1.5);
size(225pt,0,IgnoreAspect);
real f(pair z) {
return cos(2pi*z.x)*sin(2pi*z.y);
}
surface s=surface(f,(-1/2,-1/2),(1/2,1/2),20,
Spline);
draw(s,mean(palette(s.map(zpart),Rainbow())),
black);
xaxis3(Label("$x$",0.5),Bounds,InTicks);
yaxis3(Label("$y$",0.5),Bounds,InTicks);
zaxis3(Label("$z$",0.5),Bounds,-1,1,
InTicks(trailingzero));
grid3(XYZgrid);