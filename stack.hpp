#ifndef  __STACK__
#define  __STACK__

#include "list.hpp"

//stackʵ�ʲ�����������һ������������ڲ�ʹ��list��Ϊ�ײ�������Ҳ��ʹ��deque��vector�ȣ������ʵ�ַǳ���
namespace lfp {
	template<class T, class container=list<T>>
	class stack {
	public:
		typedef typename Container::value_type			value_type;
		typedef typename Container::size_type			size_type;
		typedef typename Container::reference			reference;
		typedef typename Container::const_reference		const_reference;
	protected:
		Container cont;		//�ײ�����

		/* �������õײ������Ĳ���ʵ��stack����ӿ� */
	public:
		stack() : cont() { }
		bool empty() const { return cont.empty(); }
		size_type size() const { return cont.size(); }
		reference top() { return cont.back(); }
		const_reference top() const { return cont.back(); }
		void push(const value_type& x) { cont.push_back(x); }
		void pop() { cont.pop_back(); }
	};
}





#endif // ! __STACK__
