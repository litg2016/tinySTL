#pragma once
#include<iostream>
using namespace std;

template<class T>
class vector {
public:
	typedef  T   value_type;
	typedef  T* iterator;
private:
	value_type*  _data;
	size_t _size;
	size_t _capacity;
public:
	//���캯��
	vector():_data(NULL),_size(0),_capacity(0){}
	//�������캯��
	vector(const vector&v) {
		_size = v._size;
		_capacity = v._capacity;
		_data = new value_type[_capacity];
		for (int i = 0; i < _size; i++) {
			_data[i] = v._data[i];
		}
	}
	//���ι��캯��
	vector(int n, T elem) {

	}
	//���乹��
	vector(vector<T>::iterator begin, vector<T>::iterator end) {

	}

	~vector() {
		delete[] _data;
		_data = NULL;
		_size = 0;
		_capacity = 0;
	}
	//���ظ�ֵ����� 
	vector& operator=(const vector& vec) {
		if (this == &vec) {
			return* this;
		}
		value_type* temp = new value_type[vec._capacity];
		for (int i = 0; i < vec._size; i++) {
			temp[i] = vec._data[i];
		}
		delete[] _data;
		_data = temp;
		_size = vec._size;
		_capacity = vec._capacity;
		return *this;
	}
	//����[]�����
	value_type& operator[](size_t index) {
		return _data[size_t];
	}
	bool operator==(const vector& vec) {
		if (_size != vec._size) return false;
		for (int i = 0; i < _size; i++) {
			if (_data[i] != vec._data[i]) {
				return false;
			}
		}
		return true;
	}
	//��ĩβ���Ԫ��
	void push_back(value_type val) {
		if (_capacity == 0) {
			_capacity = 1;
			_data = new value_type[1];
		}
		else if (_size + 1 > _capacity) {
			_capacity *= 2;
			value_type* temp = new value_type[_capacity];
			for (int i = 0; i < _size; i++) {
				temp[i] = _data[i];
			}
			delete[] _data;
			_data = temp;
		}
		_data[_size] = val;
		_size++;
	}
	//��ָ��λ�ò���Ԫ��
	void insert(iterator it, value_type val) {
		int index = it - _data;
		if (_capacity == 0) {
			_capacity = 1;
			_data = new value_type[1];
			_data[0] = val;
		}
		else if (_size + 1 > capacity) {
			_capacity *= 2;
			value_type* temp = new value_type[_capacity];
			int i = 0;
			for (i = 0; i < index; i++) {
				temp[i] = _data[i];
			}
			temp[i] = val;
			for(i++; i < _size+1; i++) {
				temp[i] = _data[i - 1];
			}
			delete[] _data;
			_data = temp;
		}
		else {
			for (int i = _size; i>index; i--) {
				_data[i] = _data[i - 1];
			}
			_data[index] = val;

		}
	}
	//ɾ��β��Ԫ��
	void pop_back() {
		_size--;
	}

	void erase(iterator it) {
		size_t index = it - _data;
		for (int i = index; i < _size; i++) {
			_data[i] = _data[i + 1];
		}
		_size--;
	}
	value_type front() const{
		return _data[0];
	}
	value_type back() const {
		return _data[_size - 1];
	}
	//���ص�һ��Ԫ�صĵ�ַ
	iterator begin() {
		return _data;
	}
	//�������һ��Ԫ����һ���ĵ�ַ
	iterator end() {
		return _data + _size;
	}
	size_t size() {
		return _size;
	}
	size_t capacity() {
		return _capacity;
	}
	bool empty() {
		return _size == 0;
	}

};

//int main() {
//	vector<string>v;
//	v.push_back("hello world");
//	v.push_back("hi");
//	for(auto elem : v) {
//		cout << elem << endl;
//	}
//	v.pop_back();
//	cout << "after delete";
//	for (auto elem : v) {
//		cout << elem << endl;
//	}
//
//
//
//}