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
		typedef bidirectional_iterator_tag			iterator_category;		//˫�������
		typedef T									value_type;
		typedef Ptr									pointer;
		typedef const Ptr							const_pointer;
		typedef Ref									reference;
		typedef const Ref							const_reference;
		typedef size_t								size_type;
		typedef ptrdiff_t							difference_type;
		typedef __node_type<T>*  node_pointer;

		node_pointer node;		//�������ڲ�ӵ��һ��ָ�룬ָ��list�Ľڵ�

		//constructor
		__list_iterator() { }
		__list_iterator(node_pointer x) : node(x) { }
		__list_iterator(const iterator& x) : node(x.node) { }

		bool operator==(const self& x) const { return node == x.node; }
		bool operator!=(const self& x) const { return node != x.node; }
		//���¶Ե�����ȡֵ��ȡ���ǽڵ����ֵ
		reference operator*() const { return (*node).data; }
		//�����ǵ������ĳ�Ա��ȡ�����ӵı�׼����
		pointer operator->() const { return &(operator*()); }		//����*�����ȡ��node�����ݣ�����&ȡ�õ�ַ
		//�Ե�������1����ǰ��һ���ڵ�
		self& operator++() {	//ǰ��++
			node = (*node).next;
			return *this;
		}
		self operator++(int) {	//����++
			self tmp = *this;
			++*this;
			return tmp;
		}
		//�Ե�������1���Ǻ���һ���ڵ�
		self& operator--() {	//ǰ��--
			node = (*node).prev;
			return *this;
		}
		self operator--(int) {	//����--
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

		/* ���º����ֱ��������á��ͷš����졢����һ���ڵ�����һ���ڵ㲢���� */
		node_pointer get_node() { return node_type_allocator::allocate(); }
		//�ͷ�һ���ڵ�
		void put_node(node_pointer p) { node_type_allocator::deallocate(p); }
		//���ò�����һ���ڵ�
		node_pointer create_node(const T& x) {
			node_pointer p = get_node();
			construct(&p->data, x);		//�� p �� data ��Ա�ĵ�ַ������һ������
			return p;
		}
		//�������ͷ�һ���ڵ�
		void destroy_node(node_pointer p) {
			destroy(&p->data);	//���� data ��������
			put_node(p);		//�ͷ��ڴ�
		}
		//����һ��������
		void empty_initialize() {
			node = get_node();		//����һ���ڵ㣬�� node ָ����
			node->next = node;
			node->prev = node;		//node������ָ���ָ���Լ�������Ԫ��ֵ
			size_ = 0;
		}
	public:
		list() { empty_initialize(); }
		~list() {
			clear();
			destroy_node(node);
		}


		//����ṹ��һ��iterator����
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

		//�ڵ�����pos��ָλ�ò���һ���ڵ㣬���ز�����λ�õĵ�����
		iterator insert(iterator pos, const T& x);

		//�Ƴ�������pos��ָ��㣬�����Ƴ����λ�õĵ�����
		iterator erase(iterator pos);

		void push_front(const T& x) { insert(begin(), x); }
		void push_back(const T& x) { insert(end(), x); }
		void pop_front() { erase(begin()); }
		void pop_back() { erase(--end()); }

		//�����������
		void clear();
		//��ֵΪvalue������Ԫ���Ƴ�
		void remove(const T& value);
		//�Ƴ���ֵ��ͬ������Ԫ��Ϊֻʣһ��
		void unique();

	};
	//�ڵ�����pos��ָλ�ò���һ���ڵ㣬���ز�����λ�õĵ�����
	template<class T, class Alloc>
	typename list<T, Alloc>::iterator
		list<T, Alloc>::insert(iterator pos, const T& x) {
		node_pointer tmp = create_node(x);

		//����ָ�룬�� tmp ����
		tmp->next = pos.node;
		tmp->prev = pos.node->prev;
		(pos.node->prev)->next = tmp;
		pos.node->prev = tmp;
		++size_;
		return tmp;
	}

	//�Ƴ�������pos��ָ��㣬�����Ƴ����λ�õĵ�����
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

	//�����������
	template<class T, class Alloc>
	void list<T, Alloc>::clear() {
		node_pointer cur = node->next;	//begin()
		while (cur != node) {		//����ÿһ���ڵ�
			node_pointer tmp = cur;
			cur = cur->next;
			destroy_node(tmp);		//���ٽڵ�
		}
		//������ָ���������״̬
		node->next = node;
		node->prev = node;
		size_ = 0;
	}

	//��ֵΪ value ������Ԫ���Ƴ�
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

	//�Ƴ���ֵ��ͬ��������Ԫ��Ϊֻʣһ��
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
