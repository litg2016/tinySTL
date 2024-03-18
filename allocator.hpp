#ifndef  __ALLOCATOR__
#define  __ALLOCATOR__

#include <stdlib.h>		//malloc realloc free

#if 0
#include <new>
#define __THROW_BAD_ALLOC throw std::exception("bad_alloc")
#elif !defined(__THROW_BAD_ALLOC)
#include <iostream>
#define __THROW_BAD_ALLOC std::cerr << "out of memory\n"; exit(1);
#endif

namespace lfp {
	template<int inst>
	class __malloc_alloc_template {
	private:
		//���������������������������ڴ治��������ʵ�ּ��±�
		static void* oom_malloc(size_t);
		static void* oom_realloc(void*, size_t);
		static void (*__malloc_alloc_oom_handler)();

	public:
		static void* allocate(size_t n) {
			void* res = malloc(n);
			if (res == 0) res = oom_malloc(n); //malloc����ʧ�ܸ���oom_malloc
			return res;
		}

		static void deallocate(void* p, size_t n) {
			free(p);
		}
		static void* reallocate(void* p, size_t old_sz, size_t new_sz) {
			void* res = realloc(p, new_sz);
			if (res == 0) res = oom_realloc(p, new_sz);
			return res;
		}

		//�����ڴ����handler�����ؾɵ��ڴ����������
		//����һ������ָ�룬����һ������ָ��
		static void (*set_malloc_handler(void(*f)()))(){
			void (*old)() = __malloc_alloc_oom_handler;
			__malloc_alloc_oom_handler = f;
			return (old);
		} 
		

		
	};
	//�趨malloc_alloc out-of-memory handling��ֵΪ0���д��Ͷ��趨
	template<int inst>
	void(*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

	template<int inst>
	void* __malloc_alloc_template<inst>::oom_malloc(size_t n) {
		void(*my_malloc_handler)();
		void* result;

		//���ϳ����ͷš����á����ͷš�������......
		for (int i = 0; i < 32; ++i) {
			my_malloc_handler = __malloc_alloc_oom_handler;
			if (0 == my_malloc_handler)	 //���δ���崦��������ֱ���˳�
				__THROW_BAD_ALLOC;
			(*my_malloc_handler)();		 //������ô������̣���ͼ�ͷ��ڴ�
			result = ::malloc(n);		 //�ٴγ��������ڴ�
			if (result)
				return (result);
		}

		__THROW_BAD_ALLOC;
	}
	template<int inst>
	void* __malloc_alloc_template<inst>::oom_realloc(void* p, size_t n) {
		void(*my_malloc_handler)();
		void* result;

		//���ϳ����ͷš����á����ͷš�������......
		for (int i = 0; i < 32; ++i) {
			my_malloc_handler = __malloc_alloc_oom_handler;
			if (0 == my_malloc_handler)	 //���δ���崦��������ֱ���˳�
				__THROW_BAD_ALLOC;
			(*my_malloc_handler)();		 //������ô������̣���ͼ�ͷ��ڴ�
			result = ::realloc(p, n);	 //�ٴγ��������ڴ�
			if (result)
				return (result);
		}

		__THROW_BAD_ALLOC;
	}

	typedef __malloc_alloc_template<0> malloc_alloc;	//ֱ�ӽ�instָ��Ϊ0


	//-----------------------------------------------------
	//����Ϊ��һ��������
	enum { __ALIGN = 8 };							// С��������ϵ��߽�
	enum { __MAX_BYTES = 128 };						// С�����������
	enum { __NFREELISTS = __MAX_BYTES / __ALIGN };	// ��������ĸ���


	

	template<bool threads, int inst>
	class __default_alloc_template {
	private:
		static size_t ROUND_UP(size_t bytes) {
			return ((bytes + __ALIGN - 1) & ~(__ALIGN - 1));
		}
		union obj {
			union obj* free_list_link;
			char client_data[1];
		};
		static obj* volatile free_lists[__NFREELISTS]; //��������ָ������
		//���º������������С������ʹ���ĸ���������
		static size_t FREELIST_INDEX(size_t bytes) {
			return ((bytes + __ALIGN - 1) / __ALIGN - 1);
		}
	private:
		static void* refill(size_t n);
		static char* chunk_alloc(size_t size, int& nobjs);

		static char* start_free;//�ڴ����ʼλ��
		static char* end_free;
		static size_t heap_size;
	public:
		static void* allocate(size_t n);
		static void deallocate(void* p, size_t n);
		static void* reallocate(void* p, size_t old_sz, size_t new_sz);
	};
	
	typedef __default_alloc_template<false, 0> default_alloc;

	/* static data member �ĳ�ֵ�趨 */
	template<bool threads, int inst>
	char* __default_alloc_template<threads, inst>::start_free = 0;
	template<bool threads, int inst>
	char* __default_alloc_template<threads, inst>::end_free = 0;
	template<bool threads, int inst>
	size_t __default_alloc_template<threads, inst>::heap_size = 0;

	template<bool threads, int inst>
	typename __default_alloc_template<threads, inst>::obj*
		volatile __default_alloc_template<threads, inst>::free_lists[__NFREELISTS] =
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


	/* �ռ����ú��� */
	template<bool threads, int inst>
	void* __default_alloc_template<threads, inst>::allocate(size_t total_byte) {
		//����128byte�͵��õ�һ��������
		if (total_byte > (size_t)__MAX_BYTES) {
			return malloc_alloc::allocate(total_byte);
		}

		//����Ѱ��16��free list�����ʵ���һ��
		/* ��Ϊfree_lists��һ��ָ�����飬ͬʱ�����ֿ���ת��Ϊָ��ʹ�ã�����free_lists�൱��ָ��ָ���ָ�� */
		obj* volatile* pp_free_list = free_lists + FREELIST_INDEX(total_byte);
		obj* result = *pp_free_list;

		//û�ҵ����õ�free list����n�ϵ���8�ı�����׼���������free list
		if (0 == result) {
			void* r = refill(ROUND_UP(total_byte));
			return r;
		}
		//����free list��ɾ��ȡ�ߵ��Ǹ�����  ȡ�ߵ��ǿ��ǵ�һ��
		*pp_free_list = result->free_list_link;
		return result;
	}

	/* �ռ��ͷź���, p����Ϊ0 */
	template<bool threads, int inst>
	void __default_alloc_template<threads, inst>::deallocate(void* p, size_t n) {
		//����128�͵��õ�һ��������
		if (n > (size_t)__MAX_BYTES) {
			malloc_alloc::deallocate(p, n);
			return;
		}

		//����Ѱ�Ҷ�Ӧ��free list��������黹
		//������Ӧ��������ͷ��
		obj* pObj = (obj*)p;
		obj* volatile* pp_free_list = free_lists + FREELIST_INDEX(n);
		pObj->free_list_link = *pp_free_list;
		*pp_free_list = pObj;
	}

	/* �������ÿռ��С */
	template<bool threads, int inst>
	void* __default_alloc_template<threads, inst>::reallocate(void* p, size_t old_size, size_t new_size) {
		//ֱ�ӵ��õ�һ���ռ�������
		return malloc_alloc::reallocate(p, old_size, new_size);
	}

	/* �������free list������һ����СΪ size ���ڴ����Ͷ�ʹ�ã����������������� */
/* size �Ѿ������� 8 �ı���������õ���ֹһ���ڴ���������ڴ�����ӵ��� free list �� */
	template<bool threads, int inst>
	void* __default_alloc_template<threads, inst>::refill(size_t size) {
		int nobjs = 20;		//һ�λ�ȡ 20 ����СΪ size ���ڴ��
		//����chunk_alloc()���Ի�� nonjs ������
		/* ע��nobjs���Ǵ������Ҳ�Ǵ�������������ʵ�ʻ������ĸ��� */
		char* chunk = chunk_alloc(size, nobjs);

		//���ֻ���һ������������������������ߣ�free list���������
		if (1 == nobjs)
			return (chunk);

		obj* result = (obj*)chunk;	  //��һ�鷵�ظ��Ͷ�

		//׼�����free list
		obj* volatile* pp_free_list = free_lists + FREELIST_INDEX(size);
		*pp_free_list = (obj*)(chunk + size);	//�ӵڶ������鿪ʼ

		obj* cur_obj = *pp_free_list;
		for (int i = 2; i < nobjs; ++i)		//��2��ʼ����Ϊ��1���Ѿ����ظ��Ͷ�
		{
			obj* next_obj = (obj*)((char*)cur_obj + size);
			cur_obj->free_list_link = next_obj;
			cur_obj = next_obj;
		}
		cur_obj->free_list_link = 0;	//�������������һ���ָ��

		return (result);
	}
	/* ���ڴ����ȡnobjs���ڴ����飬size��ʾһ������Ĵ�С���Ѿ�������8�ı��� */
/* nobjs��ʾϣ����õ�������������Ǵ������Ҳ�Ǵ�������������ʵ�ʻ�õ�������� */
	template<bool threads, int inst>
	char* __default_alloc_template<threads, inst>::chunk_alloc(size_t size, int& nobjs) {
		char* result;
		size_t total_bytes = size * nobjs;			//����ȡ�õ��ܿռ�
		size_t bytes_left = end_free - start_free;	//�ڴ��ʣ��ռ�

		if (bytes_left >= total_bytes) {		//�ڴ�ؿռ���ȫ����������
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		else if (bytes_left >= size) {		//�ڴ��ʣ��ռ䲻����ȫ�������󣬵�������������һ������
			result = start_free;
			nobjs = (int)(bytes_left / size);	//��nobjs�޸�Ϊʵ�ʻ�õ��������
			total_bytes = size * nobjs;			//ʵ�ʻ�õ��ܿռ�
			start_free += total_bytes;
			return result;
		}
		else {		//�ڴ�ʣ������һ�����鶼�޷�����
			//��Ҫ��ϵͳȡ�ÿռ�Ĵ�С
			size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 2);

			//�����������ڴ����ʣ�����ͷ�������ü�ֵ
			if (bytes_left > 0) {
				/* ע�⣺�ڴ���еĿռ�ÿ��������8�ı�����ÿ��ȡ��Ҳ��8�ı���������ֻҪ��ʣ��Ҳһ����8�ı��� */

				//Ѱ���ʵ���free list��Ȼ���䱸��free list
				obj* volatile* pp_free_list = free_lists + FREELIST_INDEX(bytes_left);

				//����free list�����ڴ����ʣ��Ŀռ�����
				((obj*)start_free)->free_list_link = *pp_free_list;
				*pp_free_list = (obj*)start_free;
			}

			/* ����heap�Ŀռ䣬���������ڴ�� */
			start_free = (char*)malloc(bytes_to_get);
			if (0 == start_free)		//heap�ռ䲻�㣬mallocʧ��
			{
				obj* volatile* pp_free_list;
				//���������ʵ���free list����������δ�����飬�������㹻�󡱵�free list
				for (int i = size; i <= __MAX_BYTES; i += __ALIGN)
				{
					pp_free_list = free_lists + FREELIST_INDEX(i);
					obj* pObj = *pp_free_list;
					if (pObj != 0) {		//free list��������������������
						//����free list���ͷ�δ������
						*pp_free_list = pObj->free_list_link;
						start_free = (char*)pObj;
						end_free = start_free + i;

						//�ݹ�����Լ���Ϊ������nobjs
						return chunk_alloc(size, nobjs);

						//ע�⣺�κβ�����ͷ����������free list�б���
					}
				}
				end_free = 0;		//���������û���ڴ������
				//���õ�һ��������������out of memory�����ܷ���
				//����׳��쳣�����ڴ治��������ø���
				start_free = (char*)malloc_alloc::allocate(bytes_to_get);
			}

			heap_size += bytes_to_get;
			end_free = start_free + bytes_to_get;

			//�ݹ�����Լ���Ϊ������nobjs
			return chunk_alloc(size, nobjs);
		}
	}
	#define __NODE_ALLOCATER_THREADS false

    #ifdef __USE_MALLOC		//ʹ��һ���ռ�������
		typedef malloc_alloc alloc;
	#else					//ʹ�ö����ռ�������
		typedef default_alloc alloc;  //�Ƕ��̻߳�����ֱ�ӽ�instָ��Ϊ0
	#endif
	//SGI��װ�ı�׼��alloc�������ӿڣ�һ�㶼ʹ�������׼�������ӿڣ�
	//ʹ������ӿ�ʱ���ô�С���������ֽ�Ϊ��λ��������Ԫ�ش�СΪ��λ
	template<typename T, class Alloc>
	class simple_alloc {
	public:
		static T* allocate(size_t n) {
			return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T));
		}
		static T* allocate(void) {
			return (T*)Alloc::allocate(sizeof(T));
		}
		static void deallocate(T* p, size_t n) {
			if (0 != n)
				Alloc::deallocate(p, n * sizeof(T));
		}
		static void deallocate(T* p) {
			Alloc::deallocate(p, sizeof(T));
		}
	};

}




#endif















