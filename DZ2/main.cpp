#include <iostream>
#include <fstream>
#include <vector>
#include<math.h>

using namespace std;

class Data {
public:
	string first;
	string last;
	string email;
	uint64_t ad_id;

	Data(string first, string last, string email, uint64_t ad_id): first(first), last(last), email(email), ad_id(ad_id) {}
};


class Node {
public:
	uint64_t id;
	Data* dat;
	Node* parent;
	vector<Node*> next;
	int min, max;
	bool list;

	Node(uint64_t id, string first, string last, string email, uint64_t ad_id, Node* parent, int m, bool list) : id(id), parent(parent), list(list){
		dat = new Data(first, last, email, ad_id);

		if(!parent){ //root
			min = 2;
			max = 2*floor((2*(double)m - 2)/3);
		}
		else {
			if (list) {
				min = 0;
				max = 0;
			}
			else {
				min = ceil((2 * m - 1) / 3);
				max = m;
			}
		}
		next = vector<Node*>(max);
	}
};

class IndexTree {
public:
	Node* root;
	int m;


	IndexTree(int m) {
		this->m = m;
		root = nullptr;
	}

	void insert(uint64_t id, string first, string last, string email, uint64_t ad_id) {
		if (!root) {
			root = new Node(id, first, last, email, ad_id, nullptr, m, true);
			return;
		}

		Node* curr = root;
		while (curr->next.size() != 0) {
			curr = curr->findNextByKey(id);
		}
		//add value to curr list if possible
		//if not prelom
	}
};



void main() {
	//Struktura fajla: C_ID|C_F_NAME|C_L_NAME|C_EMAIL|C_AD_ID
	//C_ID primarni kljuc nad kojim se radi index
	//B* stablo reda m izmedju [3,10]

	int m;

}

void createIndex(int& m) {
	while (1) {
		cout << "Uneti red stabla: ";
		cin >> m;
		if (m >= 3 || m <= 10) break;
	}
}
