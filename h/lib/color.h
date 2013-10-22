#ifndef LIB_COLOR_H
#define LIB_COLOR_H

namespace lib{

	typedef struct color{
		int r = 0;
		int g = 0;
		int b = 0;

		color(){}
		color(int r, int g, int b):r(r), g(g), b(b){}

	} color;

}


#endif // LIB_COLOR_H