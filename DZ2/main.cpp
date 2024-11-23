#include <iostream>
#include <fstream>
#include <vector>
#include<math.h>
#include <string>

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

	Node(Node* parent) : parent(parent), keys(), data(), children() {}

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

	int insertDataToLeaf(int m, uint64_t id, Data * d) {
		int pos = findChlidForKey(id);
		if (pos == -1) return -2; //kljuc vec postoji
		if (keys.size() == m - 1 && !!parent) return -1; //pun cvor,  a nije koren
		if (keys.size() == 2 * floor((2 * m - 2) / 3) && !parent) return -1; //pun cvor, a nije koren

		if (pos == -1) return false;
		keys.insert(keys.begin() + pos, id);
		data.insert(data.begin() + pos, d);
		return 0; //Uspesno
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


	void fix(int m, Node *& root) {
		if (!this->overflow(m)) return;

		Node *tr = this;

		if(!this->parent){

			vector<uint64_t> keyUnion = this->keys;
			vector<Data*> dataUnion = this->data;

			root->keys.clear();
			root->data.clear();
			Node* new1 = new Node(root);
			Node* new2 = new Node(root);

			root->keys.push_back(keyUnion[keyUnion.size() / 2]);
			root->data.push_back(dataUnion[dataUnion.size() / 2]);

			for (int i = 0; i < keyUnion.size() / 2; i++) {
				new1->keys.push_back(keyUnion[i]);
				new1->data.push_back(dataUnion[i]);
			}

			for (int i = keyUnion.size() / 2 + 1; i < keyUnion.size(); i++) {
				new2->keys.push_back(keyUnion[i]);
				new2->data.push_back(dataUnion[i]);
			}

			int c;
			if ((c = tr->children.size()) > 0) {

				for (int i = 0; i < c / 2; i++) {
					new1->children.push_back(tr->children[i]);
					tr->children[i]->parent = new1;
				}

				for (int i = c / 2; i < c; i++) {
					new2->children.push_back(tr->children[i]);
					tr->children[i]->parent = new2;
				}
				root->children.clear();
			}

			root->children.push_back(new1);
			root->children.push_back(new2);
			return;
		}

		bool desno;
		Node* sibling = getFreeSibling(m, desno);
		int posInParent = parent->findChlidForKey(keys[0]);

		if (sibling) {
			int insPos;
			if (desno) {
				vector<uint64_t> keyUnion;
				vector<Data*> dataUnion;

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


			}
		}
	
	}


	bool insert(int m, uint64_t id, Data* newDat, Node*& root)
	{
		Node* tr = this;

		//if (tr->children.size() == 0) {
			int res = tr->insertDataToLeaf(m, id, newDat);
			if (res == -2) return false; //Kljuc vec postoji u listu
			if (res == 0) return true; //Uspesno umetnuto u list koji nije pun
			//res = -1 <=> list je pun, cak i ako je koren idtovremeno
		//}


		//Ovde je list pun
		vector<uint64_t> keyUnion;
		vector<Data*> dataUnion;

		if (!(tr->parent)) { //Koren je i list koji se deli
			int pos = 0;
			for (int i = 0; i < tr->keys.size(); i++) {
				if (id > tr->keys[i]) pos++;
				keyUnion.push_back(tr->keys[i]);
				dataUnion.push_back(tr->data[i]);
			}

			keyUnion.insert(keyUnion.begin() + pos, id);
			dataUnion.insert(dataUnion.begin() + pos, newDat);

			root->keys.clear();
			root->data.clear();
			Node* new1 = new Node(root);
			Node* new2 = new Node(root);

			root->keys.push_back(keyUnion[keyUnion.size() / 2]);
			root->data.push_back(dataUnion[dataUnion.size() / 2]);

			for (int i = 0; i < keyUnion.size() / 2; i++) {
				new1->keys.push_back(keyUnion[i]);
				new1->data.push_back(dataUnion[i]);
			}

			for (int i = keyUnion.size() / 2 + 1; i < keyUnion.size(); i++) {
				new2->keys.push_back(keyUnion[i]);
				new2->data.push_back(dataUnion[i]);
			}

			if (tr->children.size() > 0) {

				for (int i = 0; i < keyUnion.size() / 2; i++) {
					new1->children.push_back(tr->children[i]);
				}

				for (int i = keyUnion.size() / 2 + 1; i < keyUnion.size(); i++) {
					new2->children.push_back(tr->children[i]);
				}
				root->children.clear();
			}

			root->children.push_back(new1);
			root->children.push_back(new2);
			return true;
		}

		//Nije koren, probati prelivanje
		bool desno;
		Node* sibling = tr->getFreeSibling(m, desno);
		int posInParent = tr->parent->findChlidForKey(tr->keys[0]);


		if (sibling) {
			int insPos;
			if (desno) {
				int pos = 0;
				for (int i = 0; i < tr->keys.size(); i++) {
					if (id > tr->keys[i]) pos++;
					keyUnion.push_back(tr->keys[i]);
					dataUnion.push_back(tr->data[i]);
				}
				keyUnion.push_back(tr->parent->keys[posInParent]);
				dataUnion.push_back(tr->parent->data[posInParent]);
				if (id > tr->parent->keys[posInParent]) pos++;
				for (int i = 0; i < sibling->keys.size(); i++) {
					if (id > sibling->keys[i]) pos++;
					keyUnion.push_back(sibling->keys[i]);
					dataUnion.push_back(sibling->data[i]);
				}

				keyUnion.insert(keyUnion.begin() + pos, id);
				dataUnion.insert(dataUnion.begin() + pos, newDat);
				insPos = pos;
			}
			else {
				int pos = 0;
				for (int i = 0; i < sibling->keys.size(); i++) {
					if (id > sibling->keys[i]) pos++;
					keyUnion.push_back(sibling->keys[i]);
					dataUnion.push_back(sibling->data[i]);
				}
				keyUnion.push_back(tr->parent->keys[posInParent - 1]);
				dataUnion.push_back(tr->parent->data[posInParent - 1]);
				if (id > tr->parent->keys[posInParent - 1]) pos++;
				for (int i = 0; i < tr->keys.size(); i++) {
					if (id > tr->keys[i]) pos++;
					keyUnion.push_back(tr->keys[i]);
					dataUnion.push_back(tr->data[i]);
				}


				keyUnion.insert(keyUnion.begin() + pos, id);
				dataUnion.insert(dataUnion.begin() + pos, newDat);
				insPos = pos;
			}
			int64_t mid = keyUnion[keyUnion.size() / 2];
			Data* d = dataUnion[dataUnion.size() / 2];


			if (desno) {
				tr->parent->keys[posInParent] = mid;
				tr->parent->data[posInParent] = d;

				tr->keys.clear();
				tr->data.clear();
				for (int i = 0; i < keyUnion.size() / 2; i++) {
					tr->keys.push_back(keyUnion[i]);
					tr->data.push_back(dataUnion[i]);
				}

				sibling->keys.clear();
				sibling->data.clear();
				for (int i = keyUnion.size() / 2 + 1; i < keyUnion.size(); i++) {
					sibling->keys.push_back(keyUnion[i]);
					sibling->data.push_back(dataUnion[i]);
				}

				if (tr->children.size() > 0) {
					sibling->children.insert(sibling->children.begin(), tr->children[tr->children.size()-1]);
					tr->children.pop_back();
				}

				return true;

			}
			else {//prelivanje u levo

				tr->parent->keys[posInParent - 1] = mid;
				tr->parent->data[posInParent - 1] = d;


				sibling->keys.clear();
				sibling->data.clear();
				for (int i = 0; i < keyUnion.size() / 2; i++)
				{
					sibling->keys.push_back(keyUnion[i]);
					sibling->data.push_back(dataUnion[i]);
				}

				tr->keys.clear();
				tr->data.clear();
				for (int i = keyUnion.size() / 2 + 1; i < keyUnion.size(); i++) {
					tr->keys.push_back(keyUnion[i]);
					tr->data.push_back(dataUnion[i]);
				}

				if (tr->children.size() > 0) {
					tr->children.push_back(sibling->children[0]);
					sibling->children.erase(sibling->children.begin());
				}

				return true;
			}

			return true;
		}
		else {
			//nema mesta u braci - lomi se 2 u 3, i dodaje se dodatan kljuc u parent
			bool desno = posInParent < tr->parent->children.size() - 1;
			Node* sibling = desno ? tr->parent->children[posInParent + 1] : tr->parent->children[posInParent - 1];
			vector<Node*> childrenUnion = vector<Node*>();

			int insPos = 0;
			if (desno) {
				for (int i = 0; i < tr->keys.size(); i++) {
					if (id > tr->keys[i]) insPos++;
					keyUnion.push_back(tr->keys[i]);
					dataUnion.push_back(tr->data[i]);
				}
				keyUnion.push_back(tr->parent->keys[posInParent]);
				dataUnion.push_back(tr->parent->data[posInParent]);
				if (id > tr->parent->keys[posInParent]) insPos++;
				for (int i = 0; i < sibling->keys.size(); i++) {
					if (id > sibling->keys[i]) insPos++;
					keyUnion.push_back(sibling->keys[i]);
					dataUnion.push_back(sibling->data[i]);
				}
				keyUnion.insert(keyUnion.begin() + insPos, id);
				dataUnion.insert(dataUnion.begin() + insPos, newDat);

				if(tr->children.size() > 0) {
					for (int i = 0; i < tr->children.size(); i++) {
						childrenUnion.push_back(tr->children[i]);
					}
					for (int i = 0; i < sibling->children.size(); i++) {
						childrenUnion.push_back(sibling->children[i]);
					}
				}
			}
			else { //nema desnog brata, nego ima levog brata koji je pritom pun
				for (int i = 0; i < sibling->keys.size(); i++) {
					if (id > sibling->keys[i]) insPos++;
					keyUnion.push_back(sibling->keys[i]);
					dataUnion.push_back(sibling->data[i]);
				}
				keyUnion.push_back(tr->parent->keys[posInParent - 1]);
				dataUnion.push_back(tr->parent->data[posInParent - 1]);
				if (id > tr->parent->keys[posInParent - 1]) insPos++;
				for (int i = 0; i < tr->keys.size(); i++) {
					if (id > tr->keys[i]) insPos++;
					keyUnion.push_back(tr->keys[i]);
					dataUnion.push_back(tr->data[i]);
				}
				keyUnion.insert(keyUnion.begin() + insPos, id);
				dataUnion.insert(dataUnion.begin() + insPos, newDat);

				if (tr->children.size() > 0) { //listovi su na istom nivou => ako jedan brat ima sinove, ima i drugi
					for (int i = 0; i < sibling->children.size(); i++) {
						childrenUnion.push_back(sibling->children[i]);
					}
					for (int i = 0; i < tr->children.size(); i++) {
						childrenUnion.push_back(tr->children[i]);
					}
				}
			}

			int m1 = floor((2 * m - 2) / 3);
			int m2 = m1 + floor((2 * m - 1) / 3) + 1;
			//UBACIVANJE U PARENT, I DELJENJE

			tr->keys.clear();
			tr->data.clear();

			sibling->keys.clear();
			sibling->data.clear();

			Node* newn = new Node(tr->parent);
			sibling = new Node(tr->parent);

			for (int i = 0; i < m1; i++) {
				tr->keys.push_back(keyUnion[i]);
				tr->data.push_back(dataUnion[i]);
			}

			for (int i = m1 + 1; i < m2; i++) {
				newn->keys.push_back(keyUnion[i]);
				newn->data.push_back(dataUnion[i]);
			}

			for (int i = m2 + 1; i < keyUnion.size(); i++) {
				sibling->keys.push_back(keyUnion[i]);
				sibling->data.push_back(dataUnion[i]);
			}

			if (tr->children.size() > 0) {
				tr->children.clear();
				newn->children.clear();
				sibling->children.clear();

				for (int i = 0; i < m1; i++) {
					tr->children.push_back(childrenUnion[i]);
				}

				for (int i = m1 + 1; i < m2; i++) {
					newn->children.push_back(childrenUnion[i]);
				}

				for (int i = m2 + 1; i < childrenUnion.size(); i++) {
					sibling->children.push_back(childrenUnion[i]);
				}
			}

			if (desno) { //tr je levi sin, a sibling desni
				tr->parent->keys[posInParent - 1] = keyUnion[m1];
				tr->parent->data[posInParent - 1] = dataUnion[m1];

				tr->parent->keys.insert(tr->parent->keys.begin() + posInParent, keyUnion[m2]);
				tr->parent->data.insert(tr->parent->data.begin() + posInParent, dataUnion[m2]);
				tr->parent->children[posInParent] = tr;
				tr->parent->children[posInParent + 1] = newn;
				tr->parent->children.push_back(sibling);
			}
			else { //tr je desno a sibling levo
				tr->parent->keys[posInParent - 1] = keyUnion[m1];
				tr->parent->data[posInParent - 1] = dataUnion[m1];
				tr->parent->keys.insert(tr->parent->keys.begin() + posInParent, keyUnion[m2]);
				tr->parent->data.insert(tr->parent->data.begin() + posInParent, dataUnion[m2]);

				tr->parent->children[posInParent - 1] = tr;
				tr->parent->children[posInParent] = newn;
				tr->parent->children.push_back(sibling);
			}



			if (tr->parent->overflow(m)) {
				tr->parent->fix(m, root);
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
				if (pos == -1) return false; //vec postoji
				tr = tr->children[pos];
			}

			//Pokusaj da umetnes u list tr
			Data* newDat = new Data(first, last, email, ad_id);

			tr->insert(m, id, newDat, root);

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