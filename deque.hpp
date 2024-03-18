
#ifndef  __DEQUE__
#define  __DEQUE__

#include "allocator.hpp"
#include "construct.hpp"
#include "allocator.hpp"


namespace lfp {
	inline size_t __deque_buf_size(size_t buf_size, size_t val_size) {
		return (buf_size != 0) ? buf_size : ((val_size < 512) ? size_t(512 / val_size) : size_t(1));
	}


	template<class T,class Ref,class Ptr, size_t Bufsize>
	struct __deque_iterator {
	public:
		static size_t buffer_size() { return __deque_buf_size(BufSize, sizeof(T)); }
		typedef __deque_iterator<T, T&, T*, BufSize>				iterator;
		typedef __deque_iterator<T, const T&, const T*, BufSize>	const_iterator;
		typedef random_access_iterator_tag	iterator_category;
		typedef T							value_type;
		typedef Ptr							pointer;
		typedef const Ptr					const_pointer;
		typedef Ref							reference;
		typedef const Ref					const_reference;
		typedef ptrdiff_t					difference_type;
		typedef size_t						size_type;
		typedef T**							map_pointer;
		typedef __deque_iterator			self;


		//����������������
		T* cur;				//ָ�� [first,last) ��ָ���������е�һ��Ԫ�أ���ʾ��ǰ��������ָԪ��
		T* first;			//ָ�򻺳����е�ͷ
		T* last;			//ָ�򻺳����е�β����Զָ�����һ��Ԫ�ص���һλ��
		map_pointer node;	//ָ���п����е�ǰ��������ָ�ڵ�

		/* ʹ node ��Ծ���µĻ����� new_node */
		void set_node(map_pointer new_node) {
			node = new_node;
			first = *new_node;
			last = first + difference_type(buffer_size());		//ָ��ǰ���������һ��Ԫ�ص���һλ��  ???
		}

		reference operator*() const { return *cur; }
		pointer operator->() const { return &(operator*()); }

		/* ���� - ����������ڼ�������������֮��ľ��� */
		/* ע�⣺������������ this ָ����������ı� */
		//x�����ǰ����ǰ����ں�
		difference_type operator-(const self& x) const {
			return difference_type(buffer_size()) * (node - x.node - 1)	  //����������֮���ͷβ����Ļ������������Ǿ��Ա��͵ģ�
				+ (cur - first)			//�������������ָ��������Ԫ�ظ������û���������δ���ͣ�
				+ (x.last - x.cur);		//�Ҳ�����������ָ��������Ԫ�ظ������û���������δ���ͣ�
		}

		self& operator++() {
			++cur;						//�л�����һ��Ԫ��
			if (cur == last) {			//����Ѿ����ﻺ������β�ˣ����һ��Ԫ�ص���һλ�ã�
				set_node(node + 1);		//���л�����һ���������ڵ�ĵ�һ��Ԫ��
				cur = first;
			}
			return *this;
		}

		/* ����++�������Լ�ǰ�� this �Ŀ��� */
		self operator++(int) {
			self tmp = *this;
			++*this;		//����ǰ��++
			return tmp;
		}
		/* ǰ��--�������Լ���� this */
		self& operator--() {
			if (cur == first) {		//����Ѿ����ﻺ������ͷ��
				set_node(node - 1);	//���л���ǰһ���������ڵ�
				cur = last;			//cur ָ�����һ��Ԫ�ص���һλ��
			}
			--cur;			//�л���ǰһ��Ԫ��
			return *this;
		}
		/* ����--�������Լ�ǰ�� this �Ŀ��� */
		self operator--(int) {
			self tmp = *this;
			--*this;		//����ǰ��--
			return tmp;
		}

		/* ���� += �������ע�⣺+= ������ı� this ָ�� */
		self& operator+=(difference_type n) {
			difference_type offset = cur+n-first;		//����ƶ���� cur ����ڵ�ǰ first ��λ��
			if (offset >= 0 && offset < difference_type(buffer_size())) {
				cur += n;	 //Ŀ��λ���ڵ�ǰ��������
			}
			else {		//Ŀ��λ�ò��ڵ�ǰ��������
				difference_type node_offset = (offset > 0) ? offset / difference_type(buffer_size())
					: -difference_type((-offset - 1) / buffer_size()) - 1;
				//�л�����ȷ�Ļ������ڵ�
				set_node(node + node_offset);
				//�л�����ȷ��Ԫ��
				cur = first + (offset - node_offset * difference_type(buffer_size()));
			}
			return *this;
		}

		/* ���� + �������ע�⣺+ ��������ı� this ָ�� */  //??�������Ƴ���ǰ�����������ô��
		self operator+(difference_type n) const {
			self tmp = *this;
			return tmp += n;
		}
		/* ���� -= ����������� operator+= ʵ�֡�ע�⣺-= ������ı� this ָ�� */
		self& operator-=(difference_type n) {
			return *this += -n;
		}
		/* ���� - ����������� operator-= ʵ�֡�ע�⣺- ��������ı� this ָ�� */
		self  operator- (difference_type n) const {
			self tmp = *this;
			return tmp -= n;
		}
		/* ������ʣ����� operator* �� operator+�����ı� this ָ�� */
		reference operator[](difference_type n) const { return *(*this + n); }

		bool operator==(const self& x) const { return cur == x.cur; }
		bool operator!=(const self& x) const { return !(*this == x); }
		bool operator< (const self& x) const {
			return (node == x.node) ? (cur < x.cur) : (node < x.node);
		}
	};

	template<class T, class Alloc = alloc, size_t BufSize = 0>
	struct deque {
	public:			//Basic types
		typedef typename __deque_iterator<T, T&, T*, BufSize>::iterator_category	category;
		typedef typename __deque_iterator<T, T&, T*, BufSize>::iterator				iterator;
		typedef typename __deque_iterator<T, T&, T*, BufSize>::const_iterator		const_iterator;
		typedef typename __deque_iterator<T, T&, T*, BufSize>::value_type			value_type;
		typedef typename __deque_iterator<T, T&, T*, BufSize>::pointer				pointer;
		typedef typename __deque_iterator<T, T&, T*, BufSize>::const_pointer		const_pointer;
		typedef typename __deque_iterator<T, T&, T*, BufSize>::reference			reference;
		typedef typename __deque_iterator<T, T&, T*, BufSize>::const_reference		const_reference;
		typedef typename __deque_iterator<T, T&, T*, BufSize>::size_type			size_type;
		typedef typename __deque_iterator<T, T&, T*, BufSize>::difference_type		difference_type;

		size_t(*buffer_size)() = __deque_iterator<T, T&, T*, BufSize>::buffer_size;

	protected:
		typedef T** map_pointer;

		iterator start;			//ָ���һ���������ڵ�
		iterator finish;		//ָ�����һ���������ڵ�
		map_pointer map;		//ָ���п���map��map�������ռ䣬��ÿ��Ԫ�ض�ָ��ĳ�л�������������������
		size_type map_size;		//map �ܹ������ɻ������ĸ���

		//deque ר���ռ�������
	protected:
		typedef simple_alloc<value_type, Alloc> data_allocator;
		/* ��һ��Ԫ�ش�СΪ��λ�������� */
		typedef simple_alloc<pointer, Alloc> map_allocator;
		/* ��һ���ڵ�ָ���СΪ��λ�������� */

		//����ά�� deque �ṹ���ڲ�����

		void fill_initialize(size_type n, const value_type& value);
		/* ����ʵ��deque�Ľṹ������Ԫ���趨Ϊ��ֵ value */
		void create_map_and_nodes(size_type num_elem);
		/* ������������ź�deque���ڴ�ṹ�����趨��ֵ */

		/* ����������� map�����ƾ� map ���ͷ� */
		void reallocate_map(size_type nodes_to_add, bool add_at_front);

		//push_back() �ڵ�����
		void push_back_aux(const value_type& t);
		//push_back_aux() �ڵ�����
		void reserve_map_at_back(size_type nodes_to_add = 1) {
			if (nodes_to_add + 1 > map_size - (finish.node - map)) {
				//��� map β�˽ڵ�ı��ÿռ䲻��
				//��������һ�������map�����ø���ġ�����ԭ���ġ��ͷ�ԭ���ģ�
				reallocate_map(nodes_to_add, false);
			}
		}

		//push_front() �ڵ�����
		void push_front_aux(const value_type& t);
		//push_front_aux() �ڵ�����
		void reserve_map_at_front(size_type nodes_to_add = 1) {
			if (nodes_to_add > size_type(start.node - map)) {
				//��� map ǰ�˽ڵ�ı��ÿռ䲻��
				//��������һ�������map�����ø���ġ�����ԭ���ġ��ͷ�ԭ���ģ�
				reallocate_map(nodes_to_add, true);
			}
		}

		//insert() �ڵ�����
		iterator insert_aux(iterator pos, const value_type& x);

		/* deque ����ӿ� */
	public:		//Basic accessors
		deque() : start(), finish(), map(0), map_size(0) {
			create_map_and_nodes(0);		//ӵ��0��Ԫ�ء�1����������map��СΪ 3
		}
		deque(int n, const value_type& value) : start(), finish(), map(0), map_size(0) {
			fill_initialize(n, value);
		}

		iterator begin() { return start; }
		const_iterator begin() const { return start; }
		iterator end() { return finish; }
		const_iterator end() const { return finish; }
		reference operator[](size_type n) {
			return start[difference_type(n)];		//���� __deque_iterator<>::operator[]
		}
		reference front() { return *start; }		//���� __deque_iterator<>::operator*
		const_reference front() const { return *start; }		//���� __deque_iterator<>::operator*
		reference back() {
			iterator tmp = finish;
			--tmp;			//���� __deque_iterator<>::operator--
			return *tmp;	//���� __deque_iterator<>::operator*
			//�������в��ܸ�Ϊ return *(finish - 1) 
			//��Ϊ operator-(difference_type n) �Ĳ����� -- ���Ӻܶ�
		}
		const_reference back() const {
			iterator tmp = finish;
			--tmp;			//���� __deque_iterator<>::operator--
			return *tmp;	//���� __deque_iterator<>::operator*
			//�������в��ܸ�Ϊ return *(finish - 1) 
			//��Ϊ operator-(difference_type n) �Ĳ����� -- ���Ӻܶ�
		}

		size_type size() const { return finish - start; }
		//���ϵ��� __deque_iterator<>::operator-(self&)
		size_type max_size() const { return size_type(-1); }
		bool empty() const { return finish == start; }

		void push_back(const value_type& t) {
			if (finish.cur != finish.last - 1) {		//���Ļ������������������ռ䣨Ϊ��ά������ҿ����䣩
				construct(finish.cur, t);		//ֱ���ڱ��ÿռ��Ϲ���Ԫ��
				++finish.cur;		//���� finish.cur ָ�����һ��Ԫ�ص���һλ��
			}
			else {		//��󻺳���ֻʣһ��Ԫ�ر��ÿռ䣬��ԭ finish ��ĩβ���Ԫ��
				push_back_aux(t);		//��Ҫ������һ������������ʹ finish.cur ָ���»������ĵ�һ��λ��
			}
		}

		void push_front(const value_type& t) {
			if (start.cur != start.first) {		//��һ�����������б��ÿռ�
				construct(start.cur - 1, t);		//ֱ���ڱ��ÿռ��Ϲ���Ԫ��
				--start.cur;		//������һ��������ʹ��״̬
			}
			else {		//��һ���������ޱ��ÿռ�
				push_front_aux(t);		//��Ҫ������һ��������
			}
		}

		void pop_back() {
			if (finish.cur != finish.first) {	//���һ��������������һ��Ԫ��
				--finish.cur;			//����ָ�룬�൱���ų������Ԫ��
				destroy(finish.cur);	//�����Ԫ������
			}
			else {		//��󻺳���û��Ԫ�أ����ｫ���л��������ͷ�
				data_allocator::deallocate(finish.first, buffer_size());		//�ͷ����һ��������
				finish.set_node(finish.node - 1);		//���� finish ��״̬��ʹ��ָ����һ�������������һ��Ԫ��
				finish.cur = finish.last - 1;
				destroy(finish.cur);		//����Ԫ������
			}
		}

		void pop_front() {
			if (start.cur != start.last - 1) {	//��һ����������������Ԫ��
				destroy(start.cur);		//����һԪ������
				++start.cur;			//����ָ�룬�൱���ų��˵�һԪ��
			}
			else {			//��һ����������һ��Ԫ�أ����ｫ���л��������ͷ�
				destroy(start.cur);		//����һ�������������һ��Ԫ������
				data_allocator::deallocate(start.first, buffer_size());		//�ͷŵ�һ������
				start.set_node(start.node + 1);		//������������״̬��ʹ��ָ����һ���������ĵ�һ��Ԫ��
				start.cur = start.first;
			}
		}

		//���deque��ע�⣺������Ҫ����һ��������������deque�Ĳ��ԣ�Ҳ��deque�ĳ�ʼ״̬
		void clear();
		//ɾ�� pos ��ָԪ�أ�pos Ϊ�����
		iterator erase(iterator pos);
		//ɾ�� [first, last) �ڵ�����Ԫ��
		iterator erase(iterator first, iterator last);
		//�� pos ������һ��ֵΪ x ��Ԫ��
		iterator insert(iterator pos, const value_type& x);
	};
	/* fill_initialize() �����ź�deque�Ľṹ������Ԫ���趨��ֵ */
	template<class T, class Alloc, size_t BufSize>
	void deque<T, Alloc, BufSize>::fill_initialize(size_type n, const value_type& value) {
		create_map_and_nodes(n);		//������������ź�deque���ڴ�ṹ
		map_pointer cur;
		try {
			for (cur = start.node; cur < finish.node; ++cur) {
				/* ��Ϊÿ�����ͽڵ��趨��ֵ */
				uninitialized_fill(*cur, *cur + buffer_size(), value);
			}
			/* ���һ���ڵ���趨���в�ͬ����Ϊ�����ܲ����� */
			uninitialized_fill(finish.first, finish.cur, value);
		}
		catch (...) {
			/* �Ƚ��ѹ����Ԫ������ */
			for (map_pointer beg = start.node; beg < cur; ++beg)
				destroy(*cur, *cur + buffer_size());
			/* �����л������ͷ� */
			for (cur = start.node; cur <= finish.node; ++cur)
				data_allocator::deallocate(*cur, buffer_size());
			/* �� map ָ����ָ�ռ��ͷ� */
			map_allocator::deallocate(map, map_size);
			throw;
		}
	}

	/* create_map_and_nodes()�������� num_elements ��Ԫ�صĴ洢�ռ䣬�����ź�deque�Ľṹ */
	template<class T, class Alloc, size_t BufSize>
	void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements) {
		//��Ҫ�ڵ��� = (Ԫ�ظ��� / ��������С������Ԫ�ظ���) + 1������պ����������һ���ڵ���Ϊlast
		size_type num_nodes = num_elements / buffer_size() + 1;

		//һ��map���ٹ���3���ڵ㣬����������ڵ���+2��ǰ�����һ��������չ
		map_size = num_nodes + 2;
		map = map_allocator::allocate(map_size);

		//������ nstart �� nfinish ָ��map��ӵ�е�ȫ���ڵ������������
		//�����������룬��ʹͷβ���˵���������һ����ÿ���ڵ�ζ�Ӧһ��������
		map_pointer nstart = map + 1;
		map_pointer nfinish = nstart + num_nodes - 1;

		map_pointer cur;
		try {
			//Ϊmap�Ľڵ� [nstart,nfinish] ���û�������deque�Ŀ��ÿռ�Ϊ buffer_size() * (nfinish - nstart + 1)
			for (cur = nstart; cur <= nfinish; ++cur)
				*cur = data_allocator::allocate(buffer_size());
		}
		catch (...) {
			//����ȫ���ɹ����ͽ� [nstart, cur) ��������ɵĻ�����ȫ���ͷ�
			for (map_pointer b = nstart; b < cur; ++b)
				data_allocator::deallocate(*b, buffer_size());
			//�� map ָ����ָ�ռ��ͷ�
			map_allocator::deallocate(map, map_size);
			throw;
		}
		/* ���� start ָ���һ����������start.curָ���һ���������ĵ�һ��Ԫ�� */
		start.set_node(nstart);
		start.cur = start.first;

		/* ���� finish ָ�����һ����������finish.curָ�����һ��Ԫ�ص���һλ�� */
		finish.set_node(nfinish);
		//Ϊ��ά��ǰ�պ��������ԣ����δ�������� finish.cur ָ�����һ��Ԫ�ص���һ��λ�ã���ʱ finish ָ��Ŀ�����Ԫ��
		//����պ���������ǰ��˵�����һ���ڵ㣬��ʱ�� finish.cur ָ��������һ���ڵ����ʼ������ʱ finish ָ��Ŀ���û��Ԫ��
		finish.cur = finish.first + (num_elements % buffer_size());
	}


	/* ��������һ������� map */
	template<class T, class Alloc, size_t BufSize>
	void deque<T, Alloc, BufSize>::reallocate_map(size_type nodes_to_add, bool add_at_front) {
		size_type old_num_nodes = finish.node - start.node + 1;		//����֮ǰʵ�ʻ���������
		size_type new_num_nodes = old_num_nodes + nodes_to_add;

		map_pointer new_nstart;
		/* �����map�Ĵ���2��ʵ����Ҫ�Ļ������ڵ�������map��һ�뻹û���õ��� */
		if (map_size > 2 * new_num_nodes) {
			new_nstart = map + (map_size - new_num_nodes) / 2	//��������������ƶ��� map �м�λ��
				+ (add_at_front ? nodes_to_add : 0);
			/* ����������ԭ����ǰ�� */
			if (new_nstart < start.node) {
				copy(start.node, finish.node + 1, new_nstart);
				/* ��ԭ��������Ϣ��ǰ���� */
			}
			else {
				copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
				/* ��ԭ��������Ϣ��󿽱� */
			}
		}
		else {		//��������һ��ռ���Ϊ map
			size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;
			map_pointer new_map = map_allocator::allocate(new_map_size);
			new_nstart = new_map + (new_map_size - new_num_nodes) / 2
				+ (add_at_front ? nodes_to_add : 0);

			//��������ԭmap���ݿ����������ͷ�ԭmap���趨��map����ʼ��ַ�ʹ�С
			copy(start.node, finish.node + 1, new_nstart);
			map_allocator::deallocate(map, map_size);
			map = new_map;
			map_size = new_map_size;
		}
		//���� start �� finish
		start.set_node(new_nstart);
		finish.set_node(new_nstart + old_num_nodes - 1);
	}

	//push_back() �ڵ�����
	/* ֻ�е� finish.cur == finish.last - 1 �ǲŻᱻ���� */
	/* Ҳ����˵ֻ�е����һ��������ֻʣһ��Ԫ�ش洢�ռ��˲Żᱻ���� */
	template<class T, class Alloc, size_t BufSize>
	void deque<T, Alloc, BufSize>::push_back_aux(const value_type& t) {
		value_type t_copy = t;
		reserve_map_at_back();		//�� map �ı��ÿռ�������ʱ���޷������ӻ������ڵ㣬���뻻һ������� map
		*(finish.node + 1) = data_allocator::allocate(buffer_size());		//����һ���»������ڵ�
		try {
			construct(finish.cur, t_copy);		//��ԭʼ finish ��ָ�����������һ���洢λ�ù���Ԫ��
			finish.set_node(finish.node + 1);	//���� finish ��ָ�ڵ�
			finish.cur = finish.first;			//���� finish ��״̬
		}
		catch (...) {		//�����쳣���������õĽڵ�(finish��ָ�ڵ����һ���ڵ�)�ͷ�
			data_allocator::deallocate(*(finish.node + 1));
			throw;
		}
	}

	//push_front() �ڵ�����
	/* ֻ�е� start.cur == start.first ʱ�Żᱻ���� */
	/* Ҳ����ֻ�е���һ��������û���κα��ÿռ�ʱ�Żᱻ���� */
	template<class T, class Alloc, size_t BufSize>
	void deque<T, Alloc, BufSize>::push_front_aux(const value_type& t) {
		value_type t_copy = t;
		reserve_map_at_front();		//�� map �ı��ÿռ�������ʱ���޷������ӻ������ڵ㣬���뻻һ������� map
		*(start.node - 1) = data_allocator::allocate(buffer_size());		//����һ���»������ڵ�
		try {
			start.set_node(start.node - 1);		//���� start ָ���½ڵ�
			start.cur = start.last - 1;			//���� start ��״̬
			construct(start.cur, t_copy);		//����Ԫ��
		}
		catch (...) {		//�����쳣���ͷ��½ڵ㣬�ָ���ԭʼ״̬
			start.set_node(start.node + 1);
			start.cur = start.first;
			data_allocator::deallocate(*(start.node - 1));
			throw;
		}
	}

	//���deque��ע�⣺������Ҫ����һ��������������deque�Ĳ��ԣ�Ҳ��deque�ĳ�ʼ״̬
	template<class T, class Alloc, size_t BufSize>
	void deque<T, Alloc, BufSize>::clear() {
		/* �������ͷβ�����ÿһ��������������һ�����Ǳ����� */
		for (map_pointer node = start.node + 1; node < finish.node; ++node) {
			destroy(*node, *node + buffer_size());
			/* ���������ڵ�����Ԫ�����������õ���destroy�ĵڶ��汾 */
			data_allocator::deallocate(*node, buffer_size());
			/* �ͷŻ������ڴ� */
		}

		if (start.node != finish.node) {			//������ͷβ����������
			destroy(start.cur, start.last);			//��ͷ��������Ŀǰ����Ԫ������
			destroy(finish.first, finish.cur);		//��β��������Ŀǰ����Ԫ������
			//�����ͷŻ�������ע�⣺ͷ����������
			data_allocator::deallocate(finish.first, buffer_size());
		}
		else {		//ֻ��һ��������
			destroy(start.cur, finish.cur);		//��Ԫ��������ע�����ﲻ�ͷŻ�����
		}
		//����״̬
		start.cur = start.first;
		finish = start;
	}

	//��� pos ��ָԪ�أ�pos Ϊ�����
	template<class T, class Alloc, size_t BufSize>
	typename deque<T, Alloc, BufSize>::iterator
		deque<T, Alloc, BufSize>::erase(iterator pos) {
		iterator next = pos;
		++next;
		difference_type elem_before = pos - start;		//�����֮ǰ��Ԫ�ظ���
		if (elem_before < (finish - next)) {			//�����֮ǰ��Ԫ�رȽ��پ��ƶ������֮ǰ��Ԫ��
			copy_backward(start, pos, next);
			pop_front();		//�ƶ���ϣ���һ��Ԫ�ض��࣬�������
		}
		else {			//�����֮���Ԫ�رȽ��٣��ƶ������֮���Ԫ��
			copy(next, finish, pos);
			pop_back();			//�ƶ���ϣ����һ��Ԫ�ض��࣬�������
		}
		return start + elem_before;
	}

	//ɾ�� [first, last) �ڵ�����Ԫ��
	template<class T, class Alloc, size_t BufSize>
	typename deque<T, Alloc, BufSize>::iterator
		deque<T, Alloc, BufSize>::erase(iterator first, iterator last) {
		if (first == start && last == finish) {		//���������� deque��ֱ�ӵ��� clear �Ϳ�����
			clear();
			return finish;
		}
		else {
			difference_type len = last - first;				//������䳤��
			difference_type elem_before = first - start;	//�������ǰ����Ԫ�ظ���
			if (elem_before < (finish - last)) {			//���ǰ����Ԫ�رȽ���
				copy_backward(start, first, last);		//����ƶ�ǰ����Ԫ�أ����ǵ��������
				iterator new_start = start + len;		//���deque�������
				destroy(start, new_start);				//�ƶ���ϣ��������Ԫ������
				/* ���½�����Ļ������ͷ� */
				for (map_pointer cur = start.node; cur < new_start.node; ++cur)
					data_allocator::deallocate(*cur, buffer_size());
				start = new_start;		//�趨 deque �����
			}
			else {			//�������󷽵�Ԫ�رȽ���
				copy(last, finish, first);		//���󷽵�Ԫ����ǰ�ƶ������ǵ��������
				iterator new_finish = finish - len;		//��� deque ����β��
				destroy(new_finish, finish);			//�ƶ���ϣ��������Ԫ������
				/* ���½�����Ļ������ͷ� */
				for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
					data_allocator::deallocate(*cur, buffer_size());
				finish = new_finish;		//�趨 deque ����β��
			}
			return start + elem_before;
		}
	}

	//�� pos ������һ��ֵΪ x ��Ԫ�ء�����Ԫ�صĲ���λ��
	template<class T, class Alloc, size_t BufSize>
	typename deque<T, Alloc, BufSize>::iterator
		deque<T, Alloc, BufSize>::insert(iterator pos, const value_type& x) {
		if (pos.cur == start.cur) {			//���������� deque ����ǰ��
			push_front(x);		//ֱ�ӵ���push_front()
			return start;
		}
		else if (pos.cur == finish.cur) {		//������� deque ����β��
			push_back(x);		//����push_backȥ����
			iterator tmp = finish;
			--tmp;
			return tmp;
		}
		else {
			return insert_aux(pos, x);
		}
	}

	//insert() �ڵ�����
	template<class T, class Alloc, size_t BufSize>
	typename deque<T, Alloc, BufSize>::iterator
		deque<T, Alloc, BufSize>::insert_aux(iterator pos, const value_type& x) {
		value_type x_copy = x;
		difference_type elem_before = pos - start;		//�����֮ǰ��Ԫ�ظ���

		if (elem_before < (finish - pos)) {			//��������֮ǰ��Ԫ�ظ����Ƚ���
			iterator front_old = start;				//��¼�������ʼλ��
			push_front(front());					//����ǰ�˼���һ�����һԪ����ͬ��Ԫ��
			iterator move_front = front_old + 1;	//ԭ��ʼλ�õ�Ԫ����ѹ����ǰ�ˣ���˴�ԭ��ʼλ�õ���һλ�ÿ�ʼ�ƶ�
			copy(move_front, pos, front_old);		//�� [move_front, pos) �ڵ�Ԫ��ǰ��һ��
			--pos;		//pos ǰ��ָ�����λ��
		}
		else {			//�����֮���Ԫ�ظ����Ƚ��٣���β�˽��в���������ͬ��
			iterator back_old = finish;
			push_back(back());
			iterator move_back = back_old - 1;		//��ԭ����λ�õ�ǰһλ�ÿ�ʼ����
			copy_backward(pos, move_back, back_old);		//�ƶ�Ԫ��
			//ע�⣺��ʱ pos �Ѿ�ָ����ȷ�Ĳ���λ��
		}
		*pos = x_copy;		//�޸Ĳ���λ�õ�Ԫ��ֵ
		return pos;
	}

}


#endif // ! __DEQUE__
