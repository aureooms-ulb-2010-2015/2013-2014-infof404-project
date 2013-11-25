#ifndef LIB_PIXEL_GENERATOR
#define LIB_PIXEL_GENERATOR


namespace pixel{

	template<typename P>

	class generator{
	public:
		generator(){}
		virtual ~generator(){}
		virtual P get(size_t l, size_t c, size_t h, size_t w) = 0;
	};

	template<typename P>

	class unique_generator : public generator<P>{
	private:
		P pixel;

	public:
		unique_generator(P pixel):pixel(pixel){}
		virtual ~unique_generator(){}
		virtual P get(size_t, size_t, size_t, size_t){
			return pixel;
		};
	};


	template<typename A, typename P>

	class array_generator : public generator<P>{
	private:
		const A array;
		const size_t mh;
		const size_t mw;

	public:
		array_generator(const A& array, const size_t mh, const size_t mw) : array(array), mh(mh), mw(mw){}
		virtual ~array_generator(){}
		virtual P get(size_t l, size_t c, size_t h, size_t w){
			double a = l, b = c;
			a /= h;
			a *= mh;
			b /= w;
			b *= mw;
			size_t i = a, j = b;
			return (*array)[i * mw + j];
		};
	};

	
	static const int AVOID_OVERFLOW = 0;
	static const int NORMAL = 1;

	template<typename A, typename P>

	class square_generator : public generator<P>{
	private:
		const A array;
		const size_t n;
		const size_t mw;
		size_t ol, oc;
		int mode;

	public:
		square_generator(const A& array, const size_t mh, const size_t mw, const int mode) : array(array), n(std::min(mh, mw)), mw(mw), mode(mode){
			if(n == mw){
				oc = 0;
				ol = (mh - n) / 2;
			}
			else{
				ol = 0;
				oc = (mw - n) / 2;
			}
		}
		virtual ~square_generator(){}
		virtual P get(size_t l, size_t c, size_t h, size_t w){
			size_t i, j;
			if(mode == AVOID_OVERFLOW){
				double a = l, b = c;
				a /= h;
				a *= n;
				a += ol;
				b /= w;
				b *= n;
				b += oc;
				i = a, j = b;
			}
			else{
				l *= n;
				l /= h;
				l += ol;
				c *= n;
				c /= w;
				c += oc;
				i = l, j = c;
			}
			return (*array)[i * mw + j];
		};
	};

}

#endif