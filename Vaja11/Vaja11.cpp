#include <iostream>
#include <vector>
#include <stack>
#include <fstream>
#include <chrono>
using namespace std;

int stevilo_vozlisc;

struct pot {
	int iz_vozlisca, v_vozlisce, cena;
	bool* S = new bool[stevilo_vozlisc];
	pot* naslednji;
};

void menu() {
	cout << "Trgovski potnik - izbira:" << endl;
	cout << "1) Preberi matriko" << endl;
	cout << "2) Resi problem trgovskega potnika" << endl;
	cout << "3) Izpisi dobljen seznam nivojev" << endl;
	cout << "4) Rekunstrukcija poti" << endl;
	cout << "5) Konec" << endl;
	cout << endl;
	cout << "Izbira: ";
}

void branje(int* C[]) {

	ifstream f("graf1.txt");
	f >> stevilo_vozlisc;

	for (int i = 0; i < stevilo_vozlisc; i++) {
		C[i] = new int[stevilo_vozlisc];
	}

	int cena_povezave;
	for (int i = 0; i < stevilo_vozlisc; i++) {
		for (int j = 0; j < stevilo_vozlisc; j++) {
			f >> cena_povezave;
			C[i][j] = cena_povezave;
		}
	}

	//for (int i = 0; i < stevilo_vozlisc; i++) {
	//	for (int j = 0; j < stevilo_vozlisc; j++) {
	//		std::cout << C[i][j] << " ";
	//	}
	//	cout << endl;
	//}
}

vector<pot*> vstavi_prva_vozlisca(int* C[]) {
	vector <pot*> prve_poti;

	for (int i = 1; i < stevilo_vozlisc; i++) {
		pot *p = new pot();
		p->iz_vozlisca = i + 1;			//poti označim, da gredo od 1 do stevilo_vozlisc (čeprav so indeksi od 0 do stevilo_vozlisc-1)
		p->v_vozlisce = 1;
		p->cena = C[i][0];
		for (int j = 0; j < stevilo_vozlisc; j++) {
			p->S[j] = false;
		}

		if(!prve_poti.empty())
			prve_poti.back()->naslednji = p;

		prve_poti.push_back(p);
	}

	return prve_poti;
}

void napravi_seznam(int* C[], int vozlisce, stack <vector<pot*>> sklad, vector <pot*> &nivo) {
	vector <pot*> prejsnji_nivo = sklad.top();

	for (int i = 0; i < prejsnji_nivo.size(); i++) {
		if (prejsnji_nivo[i]->iz_vozlisca == vozlisce)		//če obe poti izhajata iz istega vozlišča, ignoriramo
			continue;
		if (!prejsnji_nivo[i]->S[vozlisce]) {				//če še podanega vozlišča ni v množici povezav... 
			pot *p = new pot();								//ustvarimo novo pot na podlagi poti prejsnjega nivoja in podanega vozlišča
			p->iz_vozlisca = vozlisce;
			p->v_vozlisce = prejsnji_nivo[i]->iz_vozlisca;
			p->cena= C[p->iz_vozlisca][p->v_vozlisce] + prejsnji_nivo[i]->cena;
			p->S = prejsnji_nivo[i]->S;
			p->S[p->v_vozlisce - 1] = true;					//za novo ustvarjeno pot označimo v množici da obstaja

			nivo.push_back(p);								//pot dodamo na nivo
		}
	}

	for (int i = 0; i < nivo.size(); i++) {							//preverimo če se kateri izmed poti v novem nivoju ujemata v "iz_vozlisca" in S
		for (int j = i+1; j < nivo.size(); i++) {
			if (nivo[i]->iz_vozlisca == nivo[j]->iz_vozlisca && nivo[i]->S == nivo[j]->S) {		//... če se, odstranimo dražjo
				if (nivo[i]->cena > nivo[j]->cena)
					nivo.erase(nivo.begin() + i);
				else
					nivo.erase(nivo.begin() + j);
			}
		}

	}

}

stack <vector<pot*>> potnik(int* C[]) {
	auto start = std::chrono::steady_clock::now();

	stack <vector<pot*>> sklad;

	vector <pot*> seznam_poti;
	seznam_poti = vstavi_prva_vozlisca(C);						//ustvarimo prvi nivo za skladu
	sklad.push(seznam_poti);									//... in ga damo na sklad

	for (int i = 2; i < stevilo_vozlisc - 1; i++) {							//sprehodimo se če vse ostale nivoje (razen zadnjega):
		vector <pot*> nivo;													//ustvarimo prazen "nivo"
		for (int vozlisce = 2; vozlisce < stevilo_vozlisc; vozlisce++) {
			napravi_seznam(C, vozlisce, sklad, nivo);						//napolnimo nivo (ustvarimo seznam) z vsemi povezavami od 2. do predzadnjega vozlišča
		}
		sklad.push(nivo);
	}

	seznam_poti.clear();				
	napravi_seznam(C, 1, sklad, seznam_poti);					//ustvarimo zadnji nivo (seznam poti) in ga dodamo na sklad
	sklad.push(seznam_poti);

	auto end = std::chrono::steady_clock::now();
	cout << "Cas trajanja algoritma: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "μs." << endl;

	return sklad;
}


void izpis_nivojev(stack <vector<pot*>> sklad) {
	vector <pot*> nivo;
	int stevec = 0;

	while(sklad.size() > 0) {
		nivo = sklad.top();

		cout << "NIVO " << sklad.size() - stevec << ".:" << endl;
		for (int j = 0; j < nivo.size(); j++) {
			cout << "iz: " << nivo[j]->iz_vozlisca << ", v: " << nivo[j]->v_vozlisce << ", cena: " << nivo[j]->cena << endl;
		}
		cout << endl;

		sklad.pop();
	}
}


int main() {
	int selection;
	bool running = true;

    int** C = new int* [stevilo_vozlisc];
	stack <vector<pot*>> sklad;

	do {
		menu();
		cin >> selection;
		switch (selection) {
		case 1:
			branje(C);
			cout << endl;
			break;
		case 2:
			sklad = potnik(C);
			cout << endl;
			break;
		case 3:
			cout << endl;
			izpis_nivojev(sklad);
			cout << endl;
			break;
		case 4:

			break;
		case 5:
			running = false;
			break;
		default:
			cout << "ERROR";
			break;
		}
	} while (running);

	return 0;
}

