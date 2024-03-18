#ifndef __ALLOC__
#define __ALLOC__
#include<iostream>
#include<cstddef>



namespace JJ {
	template<class T>
	inline T* __allocate(ptrdiff_t size, T*) {
		set_new_handler(0); //卸载目前的内存分配异常处理函数，这样就会导致一旦分配内存失败，C++就会强制性抛出std:bad_alloc异常
		T* temp = T * (::operator new(size_t(size * sizeof(T)); //只分配空间，不调用函数
		if (tmp == 0) {
			cerr << "out of memory" << endl;
		}
		return tmp;

	}

	template<class T>
	inline void _deallocate(T* buffer) {
		::delete delete(buffer)
	}

	//在ptr所指的内存上构造一个T2对象
	template<class T1, class T2>
	inline void _construct(T1* ptr, const T2& value) {
		new(ptr) T1(value); 
	}

	//析构ptr所指内存的对象
	template<class T>
	inline void _destroy(T* ptr) {
		ptr->~T();
	}

	template<class T>
	class allocator {
	public:
		typedef T			value_type;
		typedef T*			pointer;
		typedef const T*	const_pointer;
		typedef T&			reference;
		typedef const T&	const_reference;
		typedef size_t		size_type;
		typedef ptrdiff_t	difference_type;

		template<class U>
		struct rebind {
			typedef allocator<U> other;
		};


		pointer allocate(size_type n, const void* hint = 0) {
			return _allocate((difference_type)n, (pointer)0);
		}

		void deallocate(pointer p, size_type n) {
			_deallocate(p);
		}

		void construct(pointer p, const T& value) {
			_construct(p, value);
		}

		void destroy(pointer p) {
			_destroy(p);
		}

		pointer address(reference x) { return (pointer)&x; }

		const_pointer const_address(const_reference x) { return (const_pointer)&x; }

		size_type max_size() const { return size_type(UINT_MAX / sizeof(T)); }
	};
}





#endif // !__ALLOC__
