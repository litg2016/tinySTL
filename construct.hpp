#ifndef  __CONSTRUCT__
#define  __CONSTRUCT__

#include<new>

namespace lfp {
	//����������ڴ�p���������
	template<typename T1, typename T2>
	inline void construct(T1* p, const T2& value) {
		new(p) T1(value);  //placement new������������ڴ�p��������󣩣�����T1::T1(value)
	}

	//������destroy��һ�汾������һ��ָ��
	template<typename T>
	inline void destroy(T* p) {
		p->~T();
	}

	//������destroy�ڶ��汾��Ե�����Ϊchar*��wchar_t*���ػ��棨����ʲôҲ��������
	inline void destroy(char*, char*) { }
	inline void destroy(wchar_t*, wchar_t*) { }



}
#endif // ! __CONSTRUCT__
