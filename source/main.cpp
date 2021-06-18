//#include "SuffixTree.hpp"
#include "SuffixTreev2.hpp"
#include "DocumentsUtilities.hpp"
#include <chrono>

using namespace std;

int main() {
	
	std::map<char, int> alphabet;
	for (char c = 'a'; c <= 'z'; ++c) alphabet.insert(std::make_pair(c, c - 'a'));
	for (char n = '0'; n <= '9'; ++n) alphabet.insert(std::make_pair(n, 26 + n - '0'));
	alphabet.insert(std::make_pair('-', 36));
	alphabet.insert(std::make_pair('$', 37));


	/*std::unordered_map<char, int> alphabet;
	for (char c = 'a'; c <= 'z'; ++c) alphabet.insert(std::make_pair(c, c - 'a'));
	for (char n = '0'; n <= '9'; ++n) alphabet.insert(std::make_pair(n, 26 + n - '0'));
	alphabet.insert(std::make_pair('-', 36));
	alphabet.insert(std::make_pair('$', 37));*/



	vector<pair<pair<int, int>, string>> delimiters;

	ifstream dataSet;
	dataSet.open("data_set.txt");
	if (!dataSet) {
		cout << "error al abrir el archivo formatted_metadata.txt" << endl;
		exit(1);
	}

	string data_set_stree = "";
	int n_lines, i = 0;
	string id, translated_text;

	cout << "Primero ingrese la cantidad de documentos que desea cargar: ";
	cin >> n_lines;

	auto t_init = chrono::high_resolution_clock::now();
	int init_delimiter = 0;
	while (i < n_lines && dataSet >> id >> translated_text) 
	{
		++i;
		translated_text += '$';
		data_set_stree += translated_text; // agregamos un caracter terminal a cada texto
		delimiters.push_back(make_pair(
			make_pair(init_delimiter, init_delimiter + translated_text.size() - 1), id));
		init_delimiter = init_delimiter + translated_text.size();
	}

	auto t_end = chrono::high_resolution_clock::now();
	auto duration_ms = chrono::duration_cast<chrono::milliseconds>(t_end - t_init).count();

	cout << "Tiempo de lectura de los documentos: " << duration_ms << " ms" << endl;

	dataSet.close();

	SUFFIX_TREE STree(alphabet);
	std::cout << "Contruccion del Arbol de Sufijos, espere...\n";
	
	
	t_init = chrono::high_resolution_clock::now();
	STree.buildSTree(data_set_stree);
	t_end = chrono::high_resolution_clock::now();
	duration_ms = chrono::duration_cast<chrono::milliseconds>(t_end - t_init).count();

	cout << "Tiempo de construccion de la estructura en: " << duration_ms << " ms" << endl;
	std::string query;
	cout << "Ingrese una palabra de busqueda o la palabra \"EXIT\" para salir del programa...\n";
	cin.ignore(1);
	do
	{
		cout << "Ingrese la palabra: ";
		getline(cin, query);
		if(query != "EXIT")
			STree.stringMatch(query, data_set_stree, delimiters);
	} while (query != "EXIT");
	cout << "eliminando la estructura por favor espere...\n";
	/**/
	return 0;
}