#pragma once
#ifndef DOCUMENTS_UTILITIES_HPP_
#define DOCUMENTS_UTILITIES_HPP_
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <cctype>
#include <math.h>
#include <fstream>
#include <map>
#include <unordered_map>
#include <iomanip>

using namespace std;

vector<string> split(string& s, string delim) 
{
	vector<string> w;
	size_t start = 0U, end = s.find(delim);
	while (end != string::npos)
	{
		w.push_back(s.substr(start, end - start));
		start = end + delim.length();
		end = s.find(delim, start);
	}
	w.push_back(s.substr(start, end));
	return w;
}

// Aquí realizaremos una búsqueda binaria en el vector delimiters
string getIndex(vector<pair<pair<int, int>, string>>& delimiters, int index)
{
	size_t right = delimiters.size() - 1, left = 0, middle;
	while (right - left > 1) 
	{
		// de este modo para que no suceda overflow
		middle = left + (right - left) / 2;
		// O(q + occ*log(N))
		if (delimiters[middle].first.first <= index && index <= delimiters[middle].first.second)
			return delimiters[middle].second; // retornamos el índice al cual pertenece el index query

		if (delimiters[middle].first.first < index)
			left = middle;
		else
			right = middle;
	}
	
	if (delimiters[left].first.first <= index && index <= delimiters[left].first.second)
		return delimiters[left].second;

	if (delimiters[right].first.first <= index && index <= delimiters[right].first.second)
		return delimiters[right].second;

	// retornamos vacío si el índice no está dentro de los límites de la query nada
	return "";
}

string formatString(string& ss, vector<string>& stop_words, map<char, int>& alphabet)
{
	// primero transformamos el string a minúsculas
	transform(ss.begin(), ss.end(), ss.begin(), [](unsigned char c) { return std::tolower(c); });
	
	// 1. obtener todas las palabras de la cadena y guardarlas en una lista
	vector<string> words = split(ss, " "); 
	string curr_word;

	// 2. seleccionar las palabras que no sean (stop-words), y solo aceptar las letras
	//    del alfabeto enviado
	vector<string> debugged_words;
	bool is_stop_word = true;
	for (int i = 0; i < words.size(); ++i)
	{	
		is_stop_word = false; // banderilla, para verificar si la palabra es (stop-word)
		// recorremos la lista de stop-words y verificamos que la palabra no sea una
		// stop-word
		for (int j = 0; j < stop_words.size(); ++j)
		{
			if (stop_words[j] == words[i])
			{
				is_stop_word = true;
				break;
			}
		}

		// si no es (stop-word), eliminamos todos los caracteres que no pertenezcan al alfabeto
		// definido
		if (!is_stop_word)
		{
			for (int j = 0; j < words[i].size(); ++j)
				if (alphabet.find(words[i][j]) != alphabet.end())
					curr_word += words[i][j];
			// agregamos la palabra transformada a la lista de palabras depuradas
			debugged_words.push_back(curr_word);
			curr_word.clear();
		}
	}
	curr_word.clear();
	// 3. concatenamos la lista de palabras depuradas en una sola cadena para us búsqueda
	//    en el árbol de sufijos
	for (int i = 0; i < debugged_words.size(); ++i)
	{
		curr_word += debugged_words[i];
	}
	return curr_word;
}

string formatString(string& ss, vector<string>& stop_words, unordered_map<char, int>& alphabet)
{
	// primero transformamos el string a minúsculas
	transform(ss.begin(), ss.end(), ss.begin(), [](unsigned char c) { return std::tolower(c); });

	// 1. obtener todas las palabras de la cadena y guardarlas en una lista
	vector<string> words = split(ss, " ");
	string curr_word;

	// 2. seleccionar las palabras que no sean (stop-words), y solo aceptar las letras
	//    del alfabeto enviado
	vector<string> debugged_words;
	bool is_stop_word = true;
	for (int i = 0; i < words.size(); ++i)
	{
		is_stop_word = false; // banderilla, para verificar si la palabra es (stop-word)
		// recorremos la lista de stop-words y verificamos que la palabra no sea una
		// stop-word
		for (int j = 0; j < stop_words.size(); ++j)
		{
			if (stop_words[j] == words[i])
			{
				is_stop_word = true;
				break;
			}
		}

		// si no es (stop-word), eliminamos todos los caracteres que no pertenezcan al alfabeto
		// definido
		if (!is_stop_word)
		{
			for (int j = 0; j < words[i].size(); ++j)
				if (alphabet.find(words[i][j]) != alphabet.end())
					curr_word += words[i][j];
			// agregamos la palabra transformada a la lista de palabras depuradas
			debugged_words.push_back(curr_word);
			curr_word.clear();
		}
	}
	curr_word.clear();
	// 3. concatenamos la lista de palabras depuradas en una sola cadena para us búsqueda
	//    en el árbol de sufijos
	for (int i = 0; i < debugged_words.size(); ++i)
	{
		curr_word += debugged_words[i];
	}
	return curr_word;
}


void showResults(map<string, int>& results, int N)
{
	if (results.empty())
	{
		cout << "No hubo resultados" << endl;
		return;
	}

	vector<pair<double, string>> TF_IDF_vec; // el vector con los scores de los documentos
	double df = static_cast<double>(results.size()), Nd = static_cast<double>(N), tf_idf;
	map<string, int>::iterator doc = results.begin();
	for (doc; doc != results.end(); ++doc)
	{
		tf_idf = static_cast<double>(doc->second) * log(Nd / df);
		TF_IDF_vec.push_back(make_pair(tf_idf, doc->first));
	}
	// ordena por el primer campo del pair, es decir por el score tf-idf
	sort(TF_IDF_vec.begin(), TF_IDF_vec.end());
	reverse(TF_IDF_vec.begin(), TF_IDF_vec.end());

	cout << "Cantidad de resultados: " << results.size() << "\n";
	cout << "      ";
	cout << setw(10);
	cout << "Score";
	cout << "  ";
	cout << setw(35);
	cout << "Link" + std::string(35/2, ' ');
	cout << "  ";
	cout << setw(35);
	cout << "PDF" + std::string(35 / 2, ' ') << "\n";
	for (int i = 0; i < TF_IDF_vec.size(); ++i)
	{
		cout << setw(5);
		cout << i + 1 << ":";
		cout << setw(10);
		cout << TF_IDF_vec[i].first;
		cout << "  ";
		cout << setw(35);
		cout << "https://arxiv.org/abs/" + TF_IDF_vec[i].second;
		cout << "  ";
		cout << setw(35);
		cout << "https://arxiv.org/pdf/" + TF_IDF_vec[i].second << endl;
	}
	return;
}

vector<string> loadStopWords() 
{
	ifstream stopWordsList;
	stopWordsList.open("stop_words_list.csv");
	if (!stopWordsList.is_open()) {
		cout << "no se pudo leer el archivo: stop_words_list.csv\n";
		return vector<string>();// retornamos un vector vacío
	}
	string stopWords;
	// solo leemos la primera línea que contiene toda la lista
	getline(stopWordsList, stopWords);

	return split(stopWords, ", ");
}


#endif // !DOCUMENTS_UTILITIES_HPP_

