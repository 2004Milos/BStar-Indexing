#include <iostream>
#include <fstream>
#include <vector>
#include<math.h>
#include <string>
#include <stack>

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
	vector<uint64_t> keys;
	vector<Data*> data;

	vector<Node*> children;
	Node* parent;
	stack<Node*> splitStack;


	Node(Node* parent) : parent(parent), keys(), data(), children(), splitStack() {}

	bool imaMesta(int m) {
		if ((this->parent) == nullptr) 
			return keys.size() < 2 * floor((2 * m - 2) / 3);
		return keys.size() < m - 1;
	}

	bool overflow(int m) {
		if ((this->parent) == nullptr) 
			return keys.size() > 2 * floor((2 * m - 2) / 3);
		return keys.size() > m - 1;
	}

	int findChlidForKey(uint64_t key) {
		int n = keys.size();
		int l = 0, r = n - 1;
		while (l <= r) {
			int m = (l + r) / 2;
			if(keys[m] == key) return -1; //kljuc vec postoji
			if (keys[0] > key) return 0;
			if (keys[n - 1] < key) return n;
			if (keys[m] < key && keys[m + 1] > key) return m + 1;
		}
	}



	Node* getFreeSibling(int m, bool& right) {
		right = false;
		if (keys.size() < m - 1) return this;

		int pos = this->parent->findChlidForKey(keys[0]) + 1; //desni brat
		if (this->parent->children.size() > pos) {
			if (this->parent->children[pos]->keys.size() < m-1) {
				right = true;
				return this->parent->children[pos];
			}
		}
		
		pos -= 2; //levi brat
		if (pos >= 0) {
			if (this->parent->children[pos]->keys.size() < m - 1) {
				return this->parent->children[pos];
			}
		}

		return nullptr;
	}


	void split(int m, Node*&root) {
		if (!parent) { //prelamanje korena
			Node * new1 = new Node(root);
			Node * new2 = new Node(root);
			if (children.size() > 0) {
				for(int i = 0; i < children.size()/2; i++) {
					new1->children.push_back(children[i]);
					children[i]->parent = new1;
				}
				for (int i = children.size() / 2; i < children.size(); i++) {
					new2->children.push_back(children[i]);
					children[i]->parent = new2;
				}
			}
			root->children.clear();
			root->children.push_back(new1);
			root->children.push_back(new2);

			int s = root->keys.size();

			for (int i = 0; i < s / 2; i++) {
				new1->keys.push_back(keys[i]);
				new1->data.push_back(data[i]);
			}
			for (int i = s / 2 + 1; i < s; i++) {
				new2->keys.push_back(keys[i]);
				new2->data.push_back(data[i]);
			}
			uint64_t mid = keys[s / 2];
			Data* midData = data[s / 2];
			keys.clear();
			data.clear();
			keys.push_back(mid);
			data.push_back(midData);
			
			return;
		}

		//Nije koren - bice split 2 u 3 ili prelivanje
		int posInParent = parent->findChlidForKey(keys[0]);

		bool desno;
		Node* freeSibling = getFreeSibling(m, desno);
		if (freeSibling) //prelivanje
		{
			if (desno) { //prelivanje u desnog brata
				freeSibling->keys.insert(freeSibling->keys.begin(), parent->keys[posInParent]);
				freeSibling->data.insert(freeSibling->data.begin(), parent->data[posInParent]);
				parent->keys[posInParent] = keys[keys.size() - 1];
				parent->data[posInParent] = data[data.size() - 1];
				keys.pop_back();
				data.pop_back();

				freeSibling->children.insert(freeSibling->children.begin(), children[children.size() - 1]);
				children[children.size() - 1]->parent = freeSibling;
				children.pop_back();
			}
			else { //prelivanje u levog brata
				freeSibling->keys.push_back(parent->keys[posInParent - 1]);
				freeSibling->data.push_back(parent->data[posInParent - 1]);
				parent->keys[posInParent - 1] = keys[0];
				parent->data[posInParent - 1] = data[0];
				keys.erase(keys.begin());
				data.erase(data.begin());

				freeSibling->children.push_back(children[0]);
				children[0]->parent = freeSibling;
				children.erase(children.begin());
			}
		}
		else { //prelamanje 2 u 3

			bool desno = posInParent < parent->children.size() - 1;
			Node* sibling = parent->children[desno ? posInParent + 1 : posInParent - 1];

			vector<uint64_t> keyUnion = vector<uint64_t>();
			vector<Data*> dataUnion = vector<Data*>();
			vector<Node*> childrenUnion = vector<Node*>();

			if (desno) {
				for (int i = 0; i < keys.size(); i++) {
					keyUnion.push_back(keys[i]);
					dataUnion.push_back(data[i]);
				}
				keyUnion.push_back(parent->keys[posInParent]);
				dataUnion.push_back(parent->data[posInParent]);
				for (int i = 0; i < sibling->keys.size(); i++) {
					keyUnion.push_back(sibling->keys[i]);
					dataUnion.push_back(sibling->data[i]);
				}

				//children union
				for (int i = 0; i < this->children.size(); i++) {
					childrenUnion.push_back(this->children[i]);
				}
				for (int i = 0; i < sibling->children.size(); i++) {
					childrenUnion.push_back(sibling->children[i]);
				}
			}
			else {
				for (int i = 0; i < sibling->keys.size(); i++) {
					keyUnion.push_back(sibling->keys[i]);
					dataUnion.push_back(sibling->data[i]);
				}
				keyUnion.push_back(parent->keys[posInParent - 1]);
				dataUnion.push_back(parent->data[posInParent - 1]);
				for (int i = 0; i < keys.size(); i++) {
					keyUnion.push_back(keys[i]);
					dataUnion.push_back(data[i]);
				}

				//children union
				for (int i = 0; i < sibling->children.size(); i++) {
					childrenUnion.push_back(sibling->children[i]);
				}
				for (int i = 0; i < this->children.size(); i++) {
					childrenUnion.push_back(this->children[i]);
				}
			}

			Node* newn = new Node(parent);
			int m1 = floor((2 * m - 2) / 3);
			int m2 = m1 + floor((2 * m - 1) / 3) + 1;

			this->keys.clear();
			this->data.clear();
			sibling->keys.clear();
			sibling->data.clear();

			if (desno) {
				for (int i = 0; i < m1; i++) {
					keys.push_back(keyUnion[i]);
					data.push_back(dataUnion[i]);
				}
				parent->keys[posInParent] = keyUnion[m1];
				parent->data[posInParent] = dataUnion[m1];
				for (int i = m1 + 1; i < m2; i++) {
					newn->keys.push_back(keyUnion[i]);
					newn->data.push_back(dataUnion[i]);
				}
				parent->keys.insert(parent->keys.begin() + posInParent + 1, keyUnion[m2]);
				parent->data.insert(parent->data.begin() + posInParent + 1, dataUnion[m2]);
				for (int i = m2 + 1; i < keyUnion.size(); i++) {
					sibling->keys.push_back(keyUnion[i]);
					sibling->data.push_back(dataUnion[i]);
				}
				parent->children.insert(parent->children.begin() + posInParent + 1, newn);

				this->children.clear();
				newn->children.clear();
				sibling->children.clear();
				for(int i = 0; i < childrenUnion.size(); i++) {
					if(i <= m1) {
						children.push_back(childrenUnion[i]);
						childrenUnion[i]->parent = this;
						continue;
					}
					if(i <= m2) {
						newn->children.push_back(childrenUnion[i]);
						childrenUnion[i]->parent = newn;
						continue;
					}
					sibling->children.push_back(childrenUnion[i]);
				}
			}
			else {
				for (int i = 0; i < m1; i++) {
					sibling->keys.push_back(keyUnion[i]);
					sibling->data.push_back(dataUnion[i]);
				}
				parent->keys[posInParent - 1] = keyUnion[m1];
				parent->data[posInParent - 1] = dataUnion[m1];
				for (int i = m1 + 1; i < m2; i++) {
					newn->keys.push_back(keyUnion[i]);
					newn->data.push_back(dataUnion[i]);
				}
				parent->keys.insert(parent->keys.begin() + posInParent, keyUnion[m2]);
				parent->data.insert(parent->data.begin() + posInParent, dataUnion[m2]);
				for (int i = m2 + 1; i < keyUnion.size(); i++) {
					keys.push_back(keyUnion[i]);
					data.push_back(dataUnion[i]);
				}
				parent->children.insert(parent->children.begin() + posInParent, newn);

				sibling->children.clear();
				newn->children.clear();
				this->children.clear();
				for (int i = 0; i < childrenUnion.size(); i++) {
					if (i <= m1) {
						sibling->children.push_back(childrenUnion[i]);
						childrenUnion[i]->parent = sibling;
						continue;
					}
					if (i <= m2) {
						newn->children.push_back(childrenUnion[i]);
						childrenUnion[i]->parent = newn;
						continue;
					}
					children.push_back(childrenUnion[i]);
				}
			}

			if (parent->overflow(m)) parent->split(m, root);
		}
	}

	bool insert(int m, uint64_t id, Data* newDat, Node*& root) {
		int pos = 0;
		while (pos < keys.size() && keys[pos] < id) pos++;

		if(pos < keys.size()) if (keys[pos] == id) return false; //vec postoji


		bool imaMesta = this->imaMesta(m);
		if (imaMesta) { //Čist insert

			keys.insert(keys.begin() + pos, id);
			data.insert(data.begin() + pos, newDat);
			return true;
		}

		if (!parent) {
			keys.insert(keys.begin() + pos, id);
			data.insert(data.begin() + pos, newDat);
			if (overflow(m)) split(m, root);
			return true;
		}

		bool desno;
		Node* sibling = getFreeSibling(m, desno);
		int posInParent = parent->findChlidForKey(keys[0]);

		keys.insert(keys.begin() + pos, id);
		data.insert(data.begin() + pos, newDat);

		if (!sibling) { //Prelamanje 2 u 3

			if (overflow(m)) split(m, root);
		}
		else { //Prelivanje
			if (desno) { //prelivanje u desnog brata
				sibling->keys.insert(sibling->keys.begin(), parent->keys[posInParent]);
				sibling->data.insert(sibling->data.begin(), parent->data[posInParent]);
				parent->keys[posInParent] = keys[keys.size() - 1];
				parent->data[posInParent] = data[data.size() - 1];
				keys.pop_back();
				data.pop_back();
			}
			else { //prelivanje u levog brata
				sibling->keys.push_back(parent->keys[posInParent - 1]);
				sibling->data.push_back(parent->data[posInParent - 1]);
				parent->keys[posInParent - 1] = keys[0];
				parent->data[posInParent - 1] = data[0];
				keys.erase(keys.begin());
				data.erase(data.begin());
			}
		}
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


	bool InsertIntoTree(uint64_t id, string first, string last, string email, uint64_t ad_id) {
		if (!root) { //Prazno stablo
			root = new Node(nullptr);
			root->keys.push_back(id);
			root->data.push_back(new Data(first, last, email, ad_id));
			return true;
		}
		else { //Stablo je kreirano
			Node* tr = root;
			while (tr->children.size() > 0) {
				int pos = tr->findChlidForKey(id);
				if (pos == -1)
					return false; //vec postoji
				tr = tr->children[pos];
			}

			//Pokusaj da umetnes u list tr
			Data* newDat = new Data(first, last, email, ad_id);

			return tr->insert(m, id, newDat, root);
		}
	}
};





void createIndex(IndexTree*& tree, int& m) {
	while (1) {
		cout << "Uneti red stabla: ";
		cin >> m;
		if (m >= 3 || m <= 10) break;
	}
	
	ifstream file;
	while(1) {
		cout << "Path: " << endl;
		string path;
		cin >> path;

		file = ifstream(path);
		if (!file.is_open()) {
			cout << "Neispravan fajl" << endl;
			continue;
		}
		break;
	}

	tree = new IndexTree(m);
	string l;

	while (getline(file, l)) {
		uint64_t id, ad_id;
		string first, last, email;

		
		id = stoull(l.substr(0, l.find('|')));
		l = l.substr(l.find('|') + 1);
		first = l.substr(0, l.find('|'));
		l = l.substr(l.find('|') + 1);
		last = l.substr(0, l.find('|'));
		l = l.substr(l.find('|') + 1);
		email = l.substr(0, l.find('|'));
		l = l.substr(l.find('|') + 1);
		ad_id = stoull(l);


		cout << id << " " << first << " " << last << " " << email << endl;
		
		

		tree->InsertIntoTree(id, first, last, email, ad_id);
	}
	
}

void main() {
	//Struktura fajla: C_ID|C_F_NAME|C_L_NAME|C_EMAIL|C_AD_ID
	//C_ID primarni kljuc nad kojim se radi index
	//B* stablo reda m izmedju [3,10]

	int m;
	IndexTree* tree = nullptr;
	createIndex(tree, m);
	cout << "Index kreiran" << endl;

}