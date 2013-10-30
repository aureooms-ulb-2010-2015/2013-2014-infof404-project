#ifndef LIB_EXCEPTION_H
#define LIB_EXCEPTION_H

namespace lib{

	class exception : public std::exception {
	private:
		std::string msg;
	public:
		exception(const std::string& msg):msg(msg){}
		const char* what() const noexcept {return msg.c_str();}
	};

}


#endif // LIB_EXCEPTION_H