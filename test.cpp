#include <iostream>
#include "list.hpp"
#include<iostream>
using namespace std;
int main() {
	lfp::list<int> list;
	list.push_back(5);
	list.push_back(6);

	lfp::list<int>::iterator it = list.begin();
	while (it != list.end()) {
		cout << *it << endl;
		it++;
	}

}
