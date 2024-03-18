#ifndef  __STACK__
#define  __STACK__

#include "list.hpp"

//stack实际不是容器而是一个配接器，其内部使用list作为底层容器（也可使用deque、vector等），因而实现非常简单
namespace lfp {
	template<class T, class container=list<T>>
	class stack {
	public:
		typedef typename Container::value_type			value_type;
		typedef typename Container::size_type			size_type;
		typedef typename Container::reference			reference;
		typedef typename Container::const_reference		const_reference;
	protected:
		Container cont;		//底层容器

		/* 以下利用底层容器的操作实现stack的外接口 */
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
