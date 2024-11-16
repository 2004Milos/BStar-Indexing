#include <iostream>
#include <fstream>

using namespace std;

class Node {
public:

	uint64_t id;
	string first;
	string last;
	string email;
	uint64_t ad_id;

	Node(uint64_t id, string first, string last, string email, uint64_t ad_id): id(id), first(first), last(last), email(email), ad_id(ad_id) {}
};



void main() {
	//Struktura fajla: C_ID|C_F_NAME|C_L_NAME|C_EMAIL|C_AD_ID
	//C_ID primarni kljuc nad kojim se radi index
	//B* stablo reda m izmedju [3,10]

	int m;
	cout << "Uneti red stabla: ";
	cin >> m;


	
}
