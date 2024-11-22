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
		if (keys.size() == m - 1 && !parent) return -1; //pun cvor,  a nije koren
		if (keys.size() == 2 * floor((2 * m - 2) / 3) && !!parent) return -1; //pun cvor, a nije koren

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


	bool insert(int m, uint64_t id, Data* d) {
		int pos = findChlidForKey(id);
		if (pos == -1) return false; //kljuc vec postoji

		if (keys.size() < m - 1) {
			keys.insert(keys.begin() + pos, id);
			data.insert(data.begin() + pos, d);
		}
		else {
			if (parent) {
				//Prelivanje
			}
			else {
				//Kreiraj novi koren
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


	bool insert(uint64_t id, string first, string last, string email, uint64_t ad_id) {
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
				if (pos == -1) return false;
				tr = tr->children[pos];
			}
			//tr je pronadjeni list
			//Pokusaj da umetnes, ako ne moze, prelivaj; ako vec postoji, vrati false
			Data* newDat = new Data(first, last, email, ad_id);
			int res = tr->insertDataToLeaf(m, id, newDat);
			if (res == -2) return false; //Kljuc vec postoji u listu
			if(res == 0) return true; //Uspesno umetnuto u list koji nije pun
			//res = -1 <=> list je pun, cak i ako je koren idtovremeno

			//Ovde je list pun

			//List je i koren?
			if (!tr->parent) {
				Node *newRoot = new Node(nullptr);

				newRoot->keys.push_back(tr->keys[tr->keys.size() / 2]);
				newRoot->data.push_back(tr->data[tr->data.size() / 2]);

				Node* newLeft = new Node(newRoot);
				Node* newRight = new Node(newRoot);

				for (int i = 0; i < tr->keys.size() / 2; i++) {
					newLeft->keys.push_back(tr->keys[i]);
					newLeft->data.push_back(tr->data[i]);
				}
				newRoot->children.push_back(newLeft);
				for (int i = tr->keys.size() / 2 + 1; i < tr->keys.size(); i++) {
					newRight->keys.push_back(tr->keys[i]);
					newRight->data.push_back(tr->data[i]);
				}
				newRoot->children.push_back(newRight);

				root = newRoot;
				return true;
			}

			//***********************************************//
			//List je pun? prelivanje u desnog brata, eventualno levi


			bool desno;
			Node* sibling = tr->getFreeSibling(m, desno);
			int currPosInParent = tr->parent->findChlidForKey(tr->keys[0]);

			if (desno) {
				vector<uint64_t> keyUnion;
				vector<Data*> dataUnion;
				for (int i = 0; i < tr->keys.size(); i++) {
					keyUnion.push_back(tr->keys[i]);
					dataUnion.push_back(tr->data[i]);
				}
				keyUnion.push_back(tr->parent->keys[currPosInParent]); //trenutni cvor je levi sin oca u odnosu na brata
				dataUnion.push_back(tr->parent->data[currPosInParent]);
				for (int i = 0; i < sibling->keys.size(); i++) {
					keyUnion.push_back(sibling->keys[i]);
					dataUnion.push_back(sibling->data[i]);
				}

				//TODO: PROMENITI OVO
				auto pos = lower_bound(keyUnion.begin(), keyUnion.end(), id) - keyUnion.begin();
				keyUnion.insert(keyUnion.begin() + pos, id);
				dataUnion.insert(dataUnion.begin() + pos, newDat);

				//Razdvojni kljuc:  FLOOR[(m+j + 1)/ 2]
				int m = floor((keyUnion.size()) / 2);

				//Podeliti kljuceve i podatke i zameniti razdelni kljuc u ocu

				tr->keys.clear();
				tr->data.clear();
				sibling->keys.clear();
				sibling->data.clear();
				
				for (int i = 0; i < m; i++) {
					tr->keys.push_back(keyUnion[i]);
					tr->data.push_back(dataUnion[i]);
				}

				tr->parent->keys[currPosInParent] = keyUnion[m];
				tr->parent->data[currPosInParent] = dataUnion[m];

				for (int i = m+1; i < keyUnion.size(); i++) {
					sibling->keys.push_back(keyUnion[i]);
					sibling->data.push_back(dataUnion[i]);
				}

				return true;
			}
			else if(sibling) { //Ne moze prelivanje desno, a moze levo
				//prelivanje u levog
				vector<uint64_t> keyUnion;
				vector<Data*> dataUnion;
				for (int i = 0; i < sibling->keys.size(); i++) {
					keyUnion.push_back(sibling->keys[i]);
					dataUnion.push_back(sibling->data[i]);
				}
				keyUnion.push_back(tr->parent->keys[currPosInParent-1]); //trenutni cvor je desni sin oca u odnosu na brata
				dataUnion.push_back(tr->parent->data[currPosInParent-1]);
				for (int i = 0; i < tr->keys.size(); i++) {
					keyUnion.push_back(tr->keys[i]);
					dataUnion.push_back(tr->data[i]);
				}

				//TODO: PROMENITI OVO
				auto pos = lower_bound(keyUnion.begin(), keyUnion.end(), id) - keyUnion.begin();
				keyUnion.insert(keyUnion.begin() + pos, id);
				dataUnion.insert(dataUnion.begin() + pos, newDat);

				//Razdvojni kljuc:  FLOOR[(m+j + 1)/ 2]
				int m = floor((keyUnion.size()) / 2);

				//Podeliti kljuceve i podatke i zameniti razdelni kljuc u ocu

				tr->keys.clear();
				tr->data.clear();
				sibling->keys.clear();
				sibling->data.clear();

				for (int i = 0; i < m; i++) {
					sibling->keys.push_back(keyUnion[i]);
					sibling->data.push_back(dataUnion[i]);
				}

				tr->parent->keys[currPosInParent-1] = keyUnion[m];
				tr->parent->data[currPosInParent-1] = dataUnion[m];

				for (int i = m + 1; i < keyUnion.size(); i++) {
					tr->keys.push_back(keyUnion[i]);
					tr->data.push_back(dataUnion[i]);					
				}
				return true;
			}

			//*************************************************//
			//Ne moze ni desno ni levo - seci 2 na 3
			//seci sa desnim bratom, ako on postoji, ako ne onda sa levim
			if (currPosInParent + 1 < tr->parent->children.size()) { //ima desnog
				vector<uint64_t> keyUnion;
				vector<Data*> dataUnion;
				for (int i = 0; i < tr->keys.size(); i++) {
					keyUnion.push_back(tr->keys[i]);
					dataUnion.push_back(tr->data[i]);
				}
				keyUnion.push_back(tr->parent->keys[currPosInParent]); //trenutni cvor je levi sin oca u odnosu na brata
				dataUnion.push_back(tr->parent->data[currPosInParent]);
				for (int i = 0; i < sibling->keys.size(); i++) {
					keyUnion.push_back(sibling->keys[i]);
					dataUnion.push_back(sibling->data[i]);
				}

				//Razdvojni kljucevi: m1,m2 =  FLOOR[(2m-2)/3], FLOOR[(2m-2)/3] + FLOOR[(2m-1)/3] + 1
				int m1 = floor((2 * m - 2) / 3);
				int m2 = m1 + floor((2 * m - 1) / 3) + 1;

				Node* newNode = new Node(tr->parent); //bice izmedju tr i sibling

				for (int i = 0; i < m1; i++) {
					tr->keys.push_back(keyUnion[i]);
					tr->data.push_back(dataUnion[i]);
				}
				for(int i = m1+1; i < m2; i++) {
					newNode->keys.push_back(keyUnion[i]);
					newNode->data.push_back(dataUnion[i]);
				}
				for (int i = m2+1; i < keyUnion.size(); i++) {
					sibling->keys.push_back(keyUnion[i]);
					sibling->data.push_back(dataUnion[i]);
				}

				//trenutni razdelni element iz oca vise nece biti razdelni, zamenice ga drugi, a dodace se jos jedan
				if (tr->parent->children.size() < m) {
					//Ima mesta u ocu - umece se desno od trenutnog u ocu
					tr->parent->keys[currPosInParent] = keyUnion[m1];
					tr->parent->data[currPosInParent] = dataUnion[m1];
					tr->parent->keys.insert(tr->parent->keys.begin() + currPosInParent + 1, keyUnion[m2]);
					tr->parent->data.insert(tr->parent->data.begin() + currPosInParent + 1, dataUnion[m2]);

					
					tr->parent->children.insert(tr->parent->children.begin() + currPosInParent + 1, newNode);
					return true;
				}
				else {
					//Nema mesta u ocu
					//potrebno je vrv izmeniti i oca, i njegovu bracu, i pretke. Mozda inicijalizovati novi koren


					if (!tr->parent->parent) { // parent je root
						vector<uint64_t> parentKeys(tr->parent->keys);
						vector<Data*> parentData(tr->parent->data);

						parentKeys[currPosInParent] = keyUnion[m1];
						
						int pos = 0;
						for (int i = 0; i < parentKeys.size(); i++) {
							if (parentKeys[i] > keyUnion[m2]) {
								pos = i;
								break;
							}
						}
						parentKeys.insert(parentKeys.begin() + pos, keyUnion[m2]);
						parentData.insert(parentData.begin() + pos, dataUnion[m2]);

						int mid = floor(parentKeys.size() / 2);

						root = new Node(nullptr);
						Node* newLeft = new Node(root);
						Node* newRight = new Node(root);

						for (int i = 0; i < mid; i++) {
							newLeft->keys.push_back(parentKeys[i]);
							newLeft->data.push_back(parentData[i]);
						}
						for (int i = mid + 1; i < parentKeys.size(); i++) {
							newRight->keys.push_back(parentKeys[i]);
							newRight->data.push_back(parentData[i]);
						}

						root->keys.push_back(parentKeys[mid]);
						root->children.push_back(newLeft);
						root->children.push_back(newRight);

						return true;
					}
					else {
						//newParentSibling->parent = tr->parent->parent;

						// Propagacija prema višem roditelju
						//return insert(promotedKey, promotedData->first, promotedData->last, promotedData->email, promotedData->ad_id);

					}
				}

			}
			else {//sa levim
			}

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
		
		

		tree->insert(id, first, last, email, ad_id);
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