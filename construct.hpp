#ifndef  __CONSTRUCT__
#define  __CONSTRUCT__

#include<new>

namespace lfp {
	//在已申请的内存p处构造对象
	template<typename T1, typename T2>
	inline void construct(T1* p, const T2& value) {
		new(p) T1(value);  //placement new（在已申请的内存p处构造对象），调用T1::T1(value)
	}

	//以下是destroy第一版本，接受一个指针
	template<typename T>
	inline void destroy(T* p) {
		p->~T();
	}

	//以下是destroy第二版本针对迭代器为char*和wchar_t*的特化版（他们什么也不用做）
	inline void destroy(char*, char*) { }
	inline void destroy(wchar_t*, wchar_t*) { }



}
#endif // ! __CONSTRUCT__
