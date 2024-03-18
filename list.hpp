#ifndef  __LIST__
#define  __LIST__


#include "allocator.hpp"
#include "iterator.hpp"
#include "construct.hpp"


namespace lfp {
	template<class T>
	struct __node_type {
		typedef __node_type<T>* node_pointer;
		node_pointer prev;
		node_pointer next;
		T data;
	};

	template<class T, class Ref = T&, class Ptr = T*>
	struct __list_iterator {
	public:
		typedef __list_iterator<T, Ref, Ptr>		self;
		typedef __list_iterator<T, T&, T*>			iterator;
		typedef const __list_iterator<T, T&, T*>	const_iterator;
		typedef bidirectional_iterator_tag			iterator_category;		//双向迭代器
		typedef T									value_type;
		typedef Ptr									pointer;
		typedef const Ptr							const_pointer;
		typedef Ref									reference;
		typedef const Ref							const_reference;
		typedef size_t								size_type;
		typedef ptrdiff_t							difference_type;
		typedef __node_type<T>*  node_pointer;

		node_pointer node;		//迭代器内部拥有一个指针，指向list的节点

		//constructor
		__list_iterator() { }
		__list_iterator(node_pointer x) : node(x) { }
		__list_iterator(const iterator& x) : node(x.node) { }

		bool operator==(const self& x) const { return node == x.node; }
		bool operator!=(const self& x) const { return node != x.node; }
		//以下对迭代器取值，取的是节点的数值
		reference operator*() const { return (*node).data; }
		//以下是迭代器的成员存取运算子的标准做法
		pointer operator->() const { return &(operator*()); }		//先用*运算符取得node的内容，再用&取得地址
		//对迭代器加1就是前进一个节点
		self& operator++() {	//前置++
			node = (*node).next;
			return *this;
		}
		self operator++(int) {	//后置++
			self tmp = *this;
			++*this;
			return tmp;
		}
		//对迭代器减1就是后退一个节点
		self& operator--() {	//前置--
			node = (*node).prev;
			return *this;
		}
		self operator--(int) {	//后置--
			self tmp = *this;
			--*this;
			return tmp;
		}
	};

	template<class T, class Alloc=alloc>
	class list {
	protected:
		typedef __node_type<T> node_type;
	public:
		typedef typename __list_iterator<T, T&, T*>::node_pointer node_pointer;
		typedef typename __list_iterator<T, T&, T*>::iterator iterator;
		typedef typename __list_iterator<T, T&, T*>::const_iterator const_iterator;
		typedef typename __list_iterator<T, T&, T*>::iterator_category category;
		typedef typename __list_iterator<T, T&, T*>::value_type value_type;
		typedef typename __list_iterator<T, T&, T*>::pointer pointer;
		typedef typename __list_iterator<T, T&, T*>::const_pointer const_pointer;
		typedef typename __list_iterator<T, T&, T*>::reference reference;
		typedef typename __list_iterator<T, T&, T*>::const_reference const_reference;
		typedef typename __list_iterator<T, T&, T*>::size_type size_type;
		typedef typename __list_iterator<T, T&, T*>::difference_type difference_type;
	protected:
		size_type size_;
		node_pointer node;
		typedef simple_alloc<node_type, Alloc> node_type_allocator;

		/* 以下函数分别用来配置、释放、构造、销毁一个节点配置一个节点并传回 */
		node_pointer get_node() { return node_type_allocator::allocate(); }
		//释放一个节点
		void put_node(node_pointer p) { node_type_allocator::deallocate(p); }
		//配置并构造一个节点
		node_pointer create_node(const T& x) {
			node_pointer p = get_node();
			construct(&p->data, x);		//在 p 的 data 成员的地址处构造一个对象
			return p;
		}
		//析构并释放一个节点
		void destroy_node(node_pointer p) {
			destroy(&p->data);	//销毁 data 处的内容
			put_node(p);		//释放内存
		}
		//产生一个空链表
		void empty_initialize() {
			node = get_node();		//配置一个节点，令 node 指向它
			node->next = node;
			node->prev = node;		//node的两个指针均指向自己，不设元素值
			size_ = 0;
		}
	public:
		list() { empty_initialize(); }
		~list() {
			clear();
			destroy_node(node);
		}


		//这里会构造一个iterator对象
		iterator begin() { return (*node).next; }
		const_iterator begin() const { return (*node).next; }
		iterator end() { return node; }
		const_iterator end() const { return node; }
		bool empty() const { return node->next == node; }
		size_type size() const { return size_; }

		reference front() { return *begin(); }
		const_reference front() const { return *begin(); }
		reference back() { return *(--end()); }
		const_reference back() const { return *(--end()); }

		//在迭代器pos所指位置插入一个节点，返回插入后该位置的迭代器
		iterator insert(iterator pos, const T& x);

		//移除迭代器pos所指结点，返回移除后该位置的迭代器
		iterator erase(iterator pos);

		void push_front(const T& x) { insert(begin(), x); }
		void push_back(const T& x) { insert(end(), x); }
		void pop_front() { erase(begin()); }
		void pop_back() { erase(--end()); }

		//清空整个链表
		void clear();
		//将值为value的所有元素移除
		void remove(const T& value);
		//移除数值相同的连续元素为只剩一个
		void unique();

	};
	//在迭代器pos所指位置插入一个节点，返回插入后该位置的迭代器
	template<class T, class Alloc>
	typename list<T, Alloc>::iterator
		list<T, Alloc>::insert(iterator pos, const T& x) {
		node_pointer tmp = create_node(x);

		//调整指针，将 tmp 插入
		tmp->next = pos.node;
		tmp->prev = pos.node->prev;
		(pos.node->prev)->next = tmp;
		pos.node->prev = tmp;
		++size_;
		return tmp;
	}

	//移除迭代器pos所指结点，返回移除后该位置的迭代器
	template<class T, class Alloc>
	typename list<T, Alloc>::iterator
		list<T, Alloc>::erase(iterator pos) {
		node_pointer next_node = pos.node->next;
		node_pointer prev_node = pos.node->prev;
		prev_node->next = next_node;
		next_node->prev = prev_node;
		destroy_node(pos.node);
		--size_;
		return iterator(next_node);
	}

	//清空整个链表
	template<class T, class Alloc>
	void list<T, Alloc>::clear() {
		node_pointer cur = node->next;	//begin()
		while (cur != node) {		//遍历每一个节点
			node_pointer tmp = cur;
			cur = cur->next;
			destroy_node(tmp);		//销毁节点
		}
		//将链表恢复到空链表状态
		node->next = node;
		node->prev = node;
		size_ = 0;
	}

	//将值为 value 的所有元素移除
	template<class T, class Alloc>
	void list<T, Alloc>::remove(const T& value) {
		iterator first = begin();
		iterator last = end();
		while (first != last)
		{
			if (*first == value) {
				first = erase(first);
				continue;
			}
			++first;
		}
	}

	//移除数值相同且连续的元素为只剩一个
	template<class T, class Alloc>
	void list<T, Alloc>::unique() {
		if (size_ == 0)
			return;

		iterator first = begin();
		iterator last = --end();
		while (first != last) {
			iterator tmp = first++;
			if (*first == *tmp) {
				erase(tmp);
			}
		}
	}


}




#endif // ! __LIST__
