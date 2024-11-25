#include <iostream>
#include <fstream>
#include <vector>
#include<math.h>
#include <string>
#include <stack>
#include <queue>
#include <tuple>
using namespace std;


class Data {
public:
	string first;
	string last;
	string email;
	uint64_t ad_id;

	Data(string first, string last, string email, uint64_t ad_id): first(first), last(last), email(email), ad_id(ad_id) {}
};

class Node;
static stack<Node*>* splitStack;

class Node {
public:
	vector<uint64_t> keys;
	vector<Data*> data;

	vector<Node*> children;
	Node* parent;


	Node(Node* parent) : parent(parent), keys(), data(), children() {
		if(!splitStack) splitStack = new stack<Node*>();
	}

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

	bool mozeBrisanje(int m) {
		if (!parent)
			return keys.size() > 1;
		return keys.size() > ceil((2 * (double)m - 1) / 3.0) - 1;
	}

	bool underflow(int m) {
		if (!parent) 
			return keys.size() < 1;
		return keys.size() < ceil((2 * (double)m - 1) / 3) - 1;
	
	}

	int findChlidForKey(uint64_t key) {
		int n = keys.size();
		int l = 0, r = n - 1;
		if (keys[0] > key) return 0;
		if (keys[n - 1] < key) return n;
		while (l <= r) {
			int m = (l + r) / 2;
			if(keys[m] == key) return -1; //kljuc vec postoji

			if (keys[m] < key && keys[m + 1] > key) return m + 1;

			if (keys[m] < key) l = m + 1;
			else r = m - 1;
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

			if (parent->overflow(m)) splitStack->push(parent); // parent->split(m, root);
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
			if (overflow(m)) {
				//split(m, root);
				splitStack->push(this);
				while (!splitStack->empty()) {
					Node* n = splitStack->top();
					splitStack->pop();
					n->split(m, root);
				}
			}
			return true;
		}

		bool desno;
		Node* sibling = getFreeSibling(m, desno);
		int posInParent = parent->findChlidForKey(keys[0]);

		keys.insert(keys.begin() + pos, id);
		data.insert(data.begin() + pos, newDat);

		if (!sibling) { //Prelamanje 2 u 3

			if (overflow(m)) {
				//split(m, root);
				splitStack->push(this);
				while(!splitStack->empty()){
					Node* n = splitStack->top();
					splitStack->pop();
					n->split(m, root);
				}
			}
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

	int find(uint64_t pk) {
		int n = keys.size();
		int l = 0, r = n - 1;
		if (keys[0] > pk) return -1;
		if (keys[n - 1] < pk) return -1;
		while (l <= r) {
			int m = (l + r) / 2;
			if (keys[m] == pk) return m; //kljuc postoji

			if (keys[m] < pk) l = m + 1;
			else r = m - 1;
		}
		return -1;
	}

	bool Print(uint64_t pk) {
		int pos = find(pk);
		if(pos == -1) return false;
		cout << "Pronadjen: " << endl;
		cout << "ID: "  << keys[pos] << "\nFirst name: " << data[pos]->first << "\nLast name: " << data[pos]->last << "\nEmail: " << data[pos]->email << "\nAdId: " << data[pos]->ad_id << endl;
		cout << endl;
		return true;
	}

	bool pozajmica(int m, int posInParent) { //ovo se radi nad listovima, tkd ne treba brinuti o deci
		if(posInParent + 1 < parent->children.size())
			if (this->parent->children[posInParent + 1]->mozeBrisanje(m)) { //pozajmica s desna
				this->keys.push_back(parent->keys[posInParent]);
				this->data.push_back(parent->data[posInParent]);
				parent->keys[posInParent] = parent->children[posInParent + 1]->keys[0];
				parent->data[posInParent] = parent->children[posInParent + 1]->data[0];
				parent->children[posInParent + 1]->keys.erase(parent->children[posInParent + 1]->keys.begin());
				parent->children[posInParent + 1]->data.erase(parent->children[posInParent + 1]->data.begin());
				return true;
			}
		if(posInParent-1 >= 0)
			if (this->parent->children[posInParent - 1]->mozeBrisanje(m)) { //pozajmica s leva
				this->keys.insert(this->keys.begin(), parent->keys[posInParent - 1]);
				this->data.insert(this->data.begin(), parent->data[posInParent - 1]);
				parent->keys[posInParent - 1] = parent->children[posInParent - 1]->keys[parent->children[posInParent - 1]->keys.size() - 1];
				parent->data[posInParent - 1] = parent->children[posInParent - 1]->data[parent->children[posInParent - 1]->data.size() - 1];
				parent->children[posInParent - 1]->keys.pop_back();
				parent->children[posInParent - 1]->data.pop_back();
				return true;
			}
		if(posInParent+2 < parent->children.size())
			if (this->parent->children[posInParent + 2]->mozeBrisanje(m)) { //pozajmica s desna 2., this je najlevlji
				this->keys.push_back(parent->keys[posInParent]);
				this->data.push_back(parent->data[posInParent]);
				parent->keys[posInParent] = parent->children[posInParent + 1]->keys[0];
				parent->data[posInParent] = parent->children[posInParent + 1]->data[0];
				parent->children[posInParent + 1]->keys.erase(parent->children[posInParent + 1]->keys.begin());
				parent->children[posInParent + 1]->data.erase(parent->children[posInParent + 1]->data.begin());

				parent->children[posInParent + 1]->keys.push_back(parent->keys[posInParent + 1]);
				parent->children[posInParent + 1]->data.push_back(parent->data[posInParent + 1]);
				parent->keys[posInParent + 1] = parent->children[posInParent + 2]->keys[0];
				parent->data[posInParent + 1] = parent->children[posInParent + 2]->data[0];
				parent->children[posInParent + 2]->keys.erase(parent->children[posInParent + 2]->keys.begin());
				parent->children[posInParent + 2]->data.erase(parent->children[posInParent + 2]->data.begin());

				return true;
			}
		if(posInParent-2 >= 0)
			if (this->parent->children[posInParent - 2]->mozeBrisanje(m)) { //pozajmica s leva 2.
				this->keys.insert(this->keys.begin(), parent->keys[posInParent - 1]);
				this->data.insert(this->data.begin(), parent->data[posInParent - 1]);
				parent->keys[posInParent - 1] = parent->children[posInParent - 1]->keys[parent->children[posInParent - 1]->keys.size() - 1];
				parent->data[posInParent - 1] = parent->children[posInParent - 1]->data[parent->children[posInParent - 1]->data.size() - 1];
				parent->children[posInParent - 1]->keys.pop_back();
				parent->children[posInParent - 1]->data.pop_back();

				parent->children[posInParent - 1]->keys.insert(parent->children[posInParent - 1]->keys.begin(), parent->keys[posInParent - 2]);
				parent->children[posInParent - 1]->data.insert(parent->children[posInParent - 1]->data.begin(), parent->data[posInParent - 2]);
				parent->keys[posInParent - 2] = parent->children[posInParent - 2]->keys[parent->children[posInParent - 2]->keys.size() - 1];
				parent->data[posInParent - 2] = parent->children[posInParent - 2]->data[parent->children[posInParent - 2]->data.size() - 1];
				parent->children[posInParent - 2]->keys.pop_back();
				parent->children[posInParent - 2]->data.pop_back();


				return true;
			}
		return false; //nije mogla pozajmica
	}

	void spajanje(int m, int posInParent) { //spajanje 3 u 2, radi se iskljucivo nad listovima
		vector<uint64_t> keyUnion = vector<uint64_t>();
		vector<Data*> dataUnion = vector<Data*>();
		vector<Node*> childrenUnion = vector<Node*>();
		if (posInParent - 1 >= 0 && posInParent < parent->children.size()-1) { //levi this desni
			for (int i = 0; i < parent->children[posInParent - 1]->keys.size(); i++) {
				keyUnion.push_back(parent->children[posInParent - 1]->keys[i]);
				dataUnion.push_back(parent->children[posInParent - 1]->data[i]);
			}
			keyUnion.push_back(parent->keys[posInParent - 1]);
			dataUnion.push_back(parent->data[posInParent - 1]);
			for (int i = 0; i < keys.size(); i++) {
				keyUnion.push_back(keys[i]);
				dataUnion.push_back(data[i]);
			}
			keyUnion.push_back(parent->keys[posInParent]);
			dataUnion.push_back(parent->data[posInParent]);
			for (int i = 0; i < parent->children[posInParent + 1]->keys.size(); i++) {
				keyUnion.push_back(parent->children[posInParent + 1]->keys[i]);
				dataUnion.push_back(parent->children[posInParent + 1]->data[i]);
			}

			int mid = keyUnion.size() / 2;
			parent->keys.erase(parent->keys.begin() + posInParent);
			parent->data.erase(parent->data.begin() + posInParent);
			parent->keys.erase(parent->keys.begin() + (posInParent - 1));
			parent->data.erase(parent->data.begin() + (posInParent - 1));

			parent->keys.insert(parent->keys.begin() + (posInParent - 1), keyUnion[mid]);
			parent->data.insert(parent->data.begin() + (posInParent - 1), dataUnion[mid]);

			parent->children.erase(parent->children.begin() + posInParent);

			parent->children[posInParent - 1]->keys.clear();
			parent->children[posInParent - 1]->data.clear();
			parent->children[posInParent]->keys.clear();
			parent->children[posInParent]->data.clear();
			for (int i = 0; i < mid; i++) {
				parent->children[posInParent - 1]->keys.push_back(keyUnion[i]);
				parent->children[posInParent - 1]->data.push_back(dataUnion[i]);
			}
			for (int i = mid + 1; i < keyUnion.size(); i++) {
				parent->children[posInParent]->keys.push_back(keyUnion[i]);
				parent->children[posInParent]->data.push_back(dataUnion[i]);
			}

			if (children.size() > 0) {
				for(int i = 0; i < children.size(); i++) {
					childrenUnion.push_back(children[i]);
				}
				for (int i = 0; i < parent->children[posInParent + 1]->children.size(); i++) {
					childrenUnion.push_back(parent->children[posInParent + 1]->children[i]);
				}
				parent->children[posInParent]->children.clear();
			}
		}
		else if (posInParent - 1 < 0) { //this, desni, desnji
			for (int i = 0; i < keys.size(); i++) {
				keyUnion.push_back(keys[i]);
				dataUnion.push_back(data[i]);
			}
			keyUnion.push_back(parent->keys[posInParent]);
			dataUnion.push_back(parent->data[posInParent]);
			for (int i = 0; i < parent->children[posInParent+1]->keys.size(); i++) {
				keyUnion.push_back(parent->children[posInParent + 1]->keys[i]);
				dataUnion.push_back(parent->children[posInParent + 1]->data[i]);
			}
			keyUnion.push_back(parent->keys[posInParent+1]);
			dataUnion.push_back(parent->data[posInParent+1]);
			for (int i = 0; i < parent->children[posInParent + 2]->keys.size(); i++) {
				keyUnion.push_back(parent->children[posInParent + 2]->keys[i]);
				dataUnion.push_back(parent->children[posInParent + 2]->data[i]);
			}

			int mid = keyUnion.size() / 2;
			parent->keys.erase(parent->keys.begin() + posInParent+1);
			parent->data.erase(parent->data.begin() + posInParent+1);
			parent->keys.erase(parent->keys.begin() + posInParent);
			parent->data.erase(parent->data.begin() + posInParent);

			parent->keys.insert(parent->keys.begin() + posInParent, keyUnion[mid]);
			parent->data.insert(parent->data.begin() + posInParent, dataUnion[mid]);

			parent->children.erase(parent->children.begin() + posInParent+1);

			parent->children[posInParent]->keys.clear();
			parent->children[posInParent]->data.clear();
			parent->children[posInParent+1]->keys.clear();
			parent->children[posInParent+1]->data.clear();
			for (int i = 0; i < mid; i++) {
				parent->children[posInParent]->keys.push_back(keyUnion[i]);
				parent->children[posInParent]->data.push_back(dataUnion[i]);
			}
			for (int i = mid + 1; i < keyUnion.size(); i++) {
				parent->children[posInParent + 1]->keys.push_back(keyUnion[i]);
				parent->children[posInParent + 1]->data.push_back(dataUnion[i]);
			}
		}
		else { //ovo je ok - levlji, levi, this
			for (int i = 0; i < parent->children[posInParent - 2]->keys.size(); i++) {
				keyUnion.push_back(parent->children[posInParent - 2]->keys[i]);
				dataUnion.push_back(parent->children[posInParent - 2]->data[i]);
			}
			keyUnion.push_back(parent->keys[posInParent - 2]);
			dataUnion.push_back(parent->data[posInParent - 2]);
			for (int i = 0; i < parent->children[posInParent - 1]->keys.size(); i++) {
				keyUnion.push_back(parent->children[posInParent - 1]->keys[i]);
				dataUnion.push_back(parent->children[posInParent - 1]->data[i]);
			}
			keyUnion.push_back(parent->keys[posInParent-1]);
			dataUnion.push_back(parent->data[posInParent-1]);
			for (int i = 0; i < keys.size(); i++) {
				keyUnion.push_back(keys[i]);
				dataUnion.push_back(data[i]);
			}

			int mid = keyUnion.size() / 2;

			parent->keys.erase(parent->keys.begin() + posInParent - 2, parent->keys.begin() + posInParent);
			parent->data.erase(parent->data.begin() + posInParent - 2, parent->data.begin() + posInParent);


			parent->keys.insert(parent->keys.begin() + (posInParent - 2), keyUnion[mid]);
			parent->data.insert(parent->data.begin() + (posInParent - 2), dataUnion[mid]);

			parent->children.erase(parent->children.begin() + posInParent-1);

			parent->children[posInParent - 2]->keys.clear();
			parent->children[posInParent - 2]->data.clear();
			parent->children[posInParent-1]->keys.clear();
			parent->children[posInParent-1]->data.clear();
			for (int i = 0; i < mid; i++) {
				parent->children[posInParent - 2]->keys.push_back(keyUnion[i]);
				parent->children[posInParent - 2]->data.push_back(dataUnion[i]);
			}
			for (int i = mid + 1; i < keyUnion.size(); i++) {
				parent->children[posInParent-1]->keys.push_back(keyUnion[i]);
				parent->children[posInParent-1]->data.push_back(dataUnion[i]);
			}
		}


	}

	Node * sedbenik(int pos) {
		if (pos + 1 >= children.size())
			return nullptr;
		Node* tr = children[pos + 1];
		while (tr->children.size() > 0)
			tr = tr->children[0];
		return tr;
	}

	void Delete(int m, int pos) {

		if (mozeBrisanje(m) && children.size() == 0) { //brisanje iz lista sa dovoljno kljuceva
			keys.erase(keys.begin() + pos);
			Data* d = data[pos];
			data.erase(data.begin() + pos);
			delete d;
			return;
		}

		keys.erase(keys.begin() + pos);
		data.erase(data.begin() + pos);

		if (children.size() != 0) //Obrisan kljuc iz unutrasnjeg cvora
		{
			//Uzeti kljuc sledbenik iz lista i postaviti ga na pos
			//Obrisati kljuc iz lista, i paziti na probleme koji mogu nastati
			Node * succ = sedbenik(pos);
			
			keys.insert(keys.begin() + pos, succ->keys[0]);
			data.insert(data.begin() + pos, succ->data[0]);

			succ->Delete(m, 0);
			return;
		}

		int posInParent = 0;

		for (int i = 0; i < parent->children.size(); i++) {
			if (parent->children[i] == this) {
				posInParent = i;
				break;
			}
		}

		//Brisanje iz LISTA sa premalo kljuceva
		if (underflow(m)) {
			bool pozajmio = pozajmica(m, posInParent);
			if(pozajmio) return; //Brat je imao kljuc da pozajmi

			//nema kljuca za pozajmicu kod brace
			//spajanje levi+this+desni
			spajanje(m, posInParent);
			return;
		}
		
		return;
	}

	~Node() {
		for (int i = 0; i < data.size(); i++) {
			delete data[i];
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
			if (tr->findChlidForKey(id) == -1) return false; //vec postoji
			//Pokusaj da umetnes u list tr
			Data* newDat = new Data(first, last, email, ad_id);

			return tr->insert(m, id, newDat, root);
		}
	}

	void Print() {
		stack<tuple<Node*, int>> stack;
		stack.push(make_tuple(root, 0));

		while (!stack.empty()) {
			tuple<Node*, int> el = stack.top();
			stack.pop();
			Node * n = get<0>(el);
			int lvl = get<1>(el);

			for (int i = n->children.size() - 1; i >= 0; i--)
				stack.push(make_tuple(n->children[i], lvl+1));

			for (int i = 0; i < lvl; i++) cout << "\t";
			for (int i = 0; i < n->keys.size(); i++) {
				//cout << "{";
				cout << n->keys[i]; //<< ", first: " << n->data[i]->first << ", last: " << n->data[i]->last << ", email: " << n->data[i]->email << ", " << ", adId: " << n->data[i]->ad_id <<"} ; ";
				if (i != n->keys.size()) cout << ",";
			}
			cout << endl;

		}
	}

	Node* Search(uint64_t pk, int& brKoraka) {
		Node* tr = root;
		brKoraka = 0;
		while (tr->children.size() > 0) {
			brKoraka++;
			int pos = tr->findChlidForKey(pk);
			if (pos == -1) return tr;
			tr = tr->children[pos];
		}
		brKoraka++;
		if (tr->findChlidForKey(pk) == -1) return tr;
		return nullptr;
	}

	vector<tuple<uint64_t, Data*>> SearchByName(string name, int& brKoraka) {
		vector<tuple<uint64_t, Data*>> records = vector<tuple<uint64_t, Data*>>();

		brKoraka = 0;
		stack<Node*> stack;
		stack.push(root);
		while (!stack.empty()) {
			brKoraka++;
			Node* n = stack.top();
			stack.pop();
			for (int i = 0; i < n->keys.size(); i++) {
				if (n->data[i]->first == name) records.push_back(make_tuple(n->keys[i], n->data[i]));
			}
			for (int i = 0; i < n->children.size(); i++) {
				stack.push(n->children[i]);
			}
		}

		return records;
	}

	bool DeleteFromTree(uint64_t id) {
		if (!root) { //Prazno stablo
			return false;
		}
		else { //Stablo je kreirano
			Node* tr = root;

			int pos;
			while (tr->children.size() > 0) {
				pos = tr->findChlidForKey(id);
				if (pos == -1)
					break; //nadjen cvor
				tr = tr->children[pos];
			}
			if ((pos = tr->find(id)) == -1) return false; //ne postoji

			tr->Delete(m, pos);
		}
	}

	~IndexTree() {
		if (root == nullptr) 
			return;

		stack<Node*> stack;
		stack.push(root);
		while (!stack.empty()) {
			Node* n = stack.top();
			stack.pop();
			for (int i = 0; i < n->children.size(); i++) {
				stack.push(n->children[i]);
			}
			delete n;
		}
	}
};





void createIndex(IndexTree*& tree, int& m) {
	while (1) {
		cout << "Uneti red stabla: ";
		cin >> m;
		if (m >= 3 && m <= 10) break;
		cout << "Red stabla mora biti izmedju 3 i 10" << endl;
	}
	
	ifstream file;
	while(1) {
		cout << "Path: ";
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

		tree->InsertIntoTree(id, first, last, email, ad_id);
	}

	file.close();	
}

void main() {
	//Struktura fajla: C_ID|C_F_NAME|C_L_NAME|C_EMAIL|C_AD_ID
	//C_ID primarni kljuc nad kojim se radi index
	//B* stablo reda m izmedju [3,10]

	int m;
	IndexTree* tree = nullptr;
	int oc;
	while (1) {
		cout << "\n1\tKreiraj indeks\n2\tUnisti indeks\n3\tIspisi indeks\n4\tInsert Record\n5\tDelete Record\n6\tPretraga po primarnom kljucu\n7\tPretraga po imenu\n8\tPretraga po k primarnih kljuceva\n9\tIzlaz\n"<< endl;
		cin >> oc;

		if(!tree && oc != 1 && oc != 9) {
			cout << "Indeks nije kreiran" << endl;
			continue;
		}

		if (oc == 1) { //Create
			createIndex(tree, m);
			cout << "Index kreiran" << endl;
			continue;
		}
		if (oc == 2) { //Destruct
			delete tree; //Poziva destruktor sa preorder prolaskom kroz stablo
			tree = nullptr;
			cout << "Index unisten" << endl;
			continue;
		}
		if (oc == 3) { //Print
			tree->Print();
			continue;
		}
		if (oc == 4) { //Insert
			uint64_t id, ad_id;
			string first, last, email;
			cout << "Id: ";
			cin >> id;
			cout << "First name: ";
			cin >> first;
			cout << "Last name: ";
			cin >> last;
			cout << "Email: ";
			cin >> email;
			cout << "Ad id: ";
			cin >> ad_id;

			if (tree->InsertIntoTree(id, first, last, email, ad_id)) cout << "Uspesno dodato" << endl;
			else cout << "Vec postoji" << endl;
			continue;
		}
		if (oc == 5) { //Delete
			uint64_t id;
			cout << "Id: ";
			cin >> id;
			if (tree->DeleteFromTree(id)) cout << "Uspesno obrisano" << endl;
			else cout << "Ne postoji" << endl;
			continue;
		}
		if (oc == 6) { //Search
			uint64_t pk;
			cout << "Primarni kljuc: ";
			cin >> pk;
			int brKoraka = 0;
			Node* n = tree->Search(pk, brKoraka);
			if (n) {
				if (n->Print(pk)) {
					cout << "Pretraga zavrsena u " << brKoraka << " koraka" << endl;
					continue;
				}
			}
			cout << "Ne postoji" << endl;
			cout << "Pretraga zavrsena u " << brKoraka << " koraka" << endl;
			continue;
		}
		if (oc == 7) { //Search po imenu
			string ime;
			cout << "Ime: ";
			cin >> ime;
			int brKoraka = 0;
			auto records = tree->SearchByName(ime, brKoraka);
			int s = records.size();
			if (s > 0) {
				for (int i = 0; i < s; i++) {
					cout << (i + 1) << ":" << endl;
					cout << "Id: " << get<0>(records[i]) << "\nFirst name: " << get<1>(records[i])->first << "\nLast name: " << get<1>(records[i])->last << "\nEmail: " << get<1>(records[i])->email << "\nAdId: " << get<1>(records[i])->ad_id << endl;
					cout << endl;
				}
			}
			else {
				cout << "Ne postoje korisnici sa tim imenom" << endl;
			}
			cout << "Pretraga zavrsena u " << brKoraka << " koraka" << endl;
			continue;
		}

	}



}