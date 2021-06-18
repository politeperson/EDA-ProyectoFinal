#pragma once
#ifndef SUFFIX_TREE_V2_HPP_
#define SUFFIX_TREE_V2_HPP_

#include <utility>
#include <string.h>
#include <memory>
#include <iostream>
#include <stack>
#include <chrono>
#include "DocumentsUtilities.hpp"

using namespace std;


class SUFFIX_NODE
{
public:
	SUFFIX_NODE();

	bool checkTransition(char letter);

	map<char, pair<pair<int, int>, shared_ptr<SUFFIX_NODE>>> getAllTransitions();

	shared_ptr<SUFFIX_NODE> setTransition(char letter, int k, int p, shared_ptr<SUFFIX_NODE> node = nullptr);

	void unsetTransition(char letter);

	shared_ptr<SUFFIX_NODE> getTransition(char letter, int& k, int& p);

	shared_ptr<SUFFIX_NODE> getSuffixLink();

	void setSuffixLink(shared_ptr<SUFFIX_NODE> _suffix_link);

	void clearTrans();

	~SUFFIX_NODE();
private:
	// transición con un carácter como llave, dos índices y el puntero al nodo hijo
	map<char, pair< pair<int, int>, shared_ptr<SUFFIX_NODE> >> trans;
	weak_ptr<SUFFIX_NODE> suffix_link;
};

SUFFIX_NODE::SUFFIX_NODE()
{
	// 64 bits: [00,...,$,-,0-9,a-z]
	// first 26 bits are for lowercase english letters [a-z]: from 0 to 25 positions
	// next 10 bits are for numbers 10-base digits [0-9]: from 26 to 35 positions
	// next 1 bit is for '-' symbol: 36 position
	// next 1 bit is for '$' symbol (terminal symbol): 37 positions
	trans.clear();
	suffix_link.reset();
}

bool SUFFIX_NODE::checkTransition(char letter)
{
	return trans.find(letter) != trans.end();
}

shared_ptr<SUFFIX_NODE> SUFFIX_NODE::setTransition(char letter, int k, int p, shared_ptr<SUFFIX_NODE> node)
{
	trans[letter] = make_pair(make_pair(k, p), node);
	return trans[letter].second; // retornamos un puntero al nuevo estado de la nueva transición creada
}

void SUFFIX_NODE::unsetTransition(char letter)
{
	trans.erase(letter);
}

shared_ptr<SUFFIX_NODE> SUFFIX_NODE::getTransition(char letter, int& k, int& p)
{
	if (trans.find(letter) != trans.end())
	{
		// cambiamos los valores de ambos índices k y p
		k = trans[letter].first.first;
		p = trans[letter].first.second;
		// retornamos el puntero al nodo de su transición
		return trans[letter].second;
	}
	// en caso de que no exista la transición retornamos una transición NULL, con índices (-1,-1)
	k = p = -1;
	return nullptr;
}

map<char, pair<pair<int, int>, shared_ptr<SUFFIX_NODE>>> SUFFIX_NODE::getAllTransitions()
{
	return trans;
}

shared_ptr<SUFFIX_NODE> SUFFIX_NODE::getSuffixLink() {
	return suffix_link.lock();
}

void SUFFIX_NODE::setSuffixLink(shared_ptr<SUFFIX_NODE> _suffix_link) {
	suffix_link = _suffix_link;
}

void SUFFIX_NODE::clearTrans() {
	trans.clear();
}

SUFFIX_NODE::~SUFFIX_NODE()
{
	// cout << "eliminando nodo con transiciones: " << trans.size() << endl;
	trans.clear();
	suffix_link.reset();
}


class SUFFIX_TREE
{
public:
	SUFFIX_TREE(map<char, int> any_alphabet);

	pair<bool, shared_ptr<SUFFIX_NODE>> testAndSplit(shared_ptr<SUFFIX_NODE>& s, int k, int p, char t, string& T);

	shared_ptr<SUFFIX_NODE> canonize(shared_ptr<SUFFIX_NODE> s, int& k, int p, string& T);

	void update(shared_ptr<SUFFIX_NODE>& s, int& k, int i, string& T);

	void buildSTree(string& T);

	void buildSTree(string& T, int& i, int& k, size_t size_T);
	// recibe el string ss que es la query, y el string dataset que es T
	void stringMatch(string& ss, string& T, vector<pair<pair<int, int>, string>>& delimiters);

	void getResults(shared_ptr<SUFFIX_NODE>& r, int k, int p, map<string, int>& results, vector<pair<pair<int, int>, string>>& delimiters);

	~SUFFIX_TREE();

private:
	shared_ptr<SUFFIX_NODE> aux_state;
	shared_ptr<SUFFIX_NODE> root_state;
	map<char, int> alphabet;
};

SUFFIX_TREE::SUFFIX_TREE(map<char, int> any_alphabet)
{
	alphabet = any_alphabet;

	// lo mismo que: aux_state = new SUFFIX_NODE;
	aux_state = make_shared<SUFFIX_NODE>();
	root_state = make_shared<SUFFIX_NODE>();

	// al inicio creamos transiciones desde el nodo auxiliar hacia la raíz, todos los caracteres
	// de nuestro alfabeto son relacionados cada una de las aristas tiene como índices a-transiciones
	// con (j, j) como índices
	for (auto& letter : alphabet)
		aux_state->setTransition(letter.first, letter.second, letter.second, root_state);
	// y la raíz tiene como suffix_link a aux_state
	root_state->setSuffixLink(aux_state);
}

shared_ptr<SUFFIX_NODE> SUFFIX_TREE::canonize(shared_ptr<SUFFIX_NODE> s, int& k, int p, string& T)
{
	if (p < k) return s; // lo mismo que return(s, k)
	// encuentra la t_k-transición g'(s, (k',p')) = s'
	int kp, pp;
	shared_ptr<SUFFIX_NODE> sp = s->getTransition(T[k], kp, pp);
	while (pp - kp <= p - k)
	{
		// k <- k + p' - k' + 1
		k = k + pp - kp + 1;
		// s <- s'
		s = sp;
		// if k <= p encuentra la t_k transición g'(s, (k',p')) = s' desde s
		if (k <= p) sp = s->getTransition(T[k], kp, pp);
	}
	return s; // return (s, k)
}

pair<bool, shared_ptr<SUFFIX_NODE>> SUFFIX_TREE::testAndSplit(shared_ptr<SUFFIX_NODE>& s, int k, int p, char t, string& T)
{
	if (k <= p)
	{
		// la t_k-transición (s, (k', p')) = s'
		int kp, pp;
		shared_ptr<SUFFIX_NODE> sp = s->getTransition(T[k], kp, pp);
		// if t = T[k' + p - k + 1] entonces return(true, s)
		if (t == T[(size_t)kp + (size_t)p - (size_t)k + (size_t)1]) return make_pair(true, s);
		// replace t_k transition above by transitions
		// g'(s, (k', k' + p - k)) = r and g'(r, (k' + p - k + 1, p')) = s'
		// where r is a new state
		// g'(s, (k', k' + p - k)) = r
		shared_ptr<SUFFIX_NODE> r = s->setTransition(T[kp], kp, kp + p - k, make_shared<SUFFIX_NODE>());
		// g'(r, (k' + p - k + 1, p')) = s'
		r->setTransition(T[(size_t)kp + (size_t)p - (size_t)k + (size_t)1], kp + p - k + 1, pp, sp);
		return make_pair(false, r);// return (false, r)
	}
	// si no hay una t-transición desde s: return (false, s)
	if (!s->checkTransition(t)) return make_pair(false, s);
	// sino return(true, s)
	return make_pair(true, s);
}

void SUFFIX_TREE::update(shared_ptr<SUFFIX_NODE>& s, int& k, int i, string& T)
{
	shared_ptr<SUFFIX_NODE> oldr = root_state; // oldr <- root; r <- nullptr
	// la variable testPair es lo equivalente a (end-point, r)
	pair<bool, shared_ptr<SUFFIX_NODE>> testPair = testAndSplit(s, k, i - 1, T[i], T);
	// while note end-point
	while (!testPair.first)
	{
		// set transition to r: g'(r, (i, infinity)) = r' donde r' es un nuevo estado
		// un estado nulo para las hojas basta, sobre todo para no ocupar espacio innecesariamente
		testPair.second->setTransition(T[i], i, INT_MAX, nullptr);// make_shared<SUFFIXNODE>()
		// f'(oldr) = r
		if (oldr != root_state) oldr->setSuffixLink(testPair.second);
		// oldr <- r
		oldr = testPair.second;
		// (s, k) = canonize(f'(s), (k, i-1))
		s = canonize(s->getSuffixLink(), k, i - 1, T);// k es pasado por referencia
		// (end-point, r) = test-and-split(s, (k, p), t_i)
		testPair = testAndSplit(s, k, i - 1, T[i], T);
	}
	if (oldr != root_state) oldr->setSuffixLink(s);
	return; // return (s, k)
}

void SUFFIX_TREE::buildSTree(string& T) {
	int k = 0, size_T = T.size();
	shared_ptr<SUFFIX_NODE> s = root_state;
	for (int i = 0; i < size_T; ++i) {
		update(s, k, i, T); // (s,k) <- update(s, (k,i))
		s = canonize(s, k, i, T); // (s,k) <- canonize(s, (k,i))
	}
}

void SUFFIX_TREE::buildSTree(string& T, int& i, int& k, size_t size_T) {
	shared_ptr<SUFFIX_NODE> s = root_state;
	for (i; i < size_T; ++i) {
		update(s, k, i, T); // (s,k) <- update(s, (k,i))
		s = canonize(s, k, i, T); // (s,k) <- canonize(s, (k,i))
	}
	// añadimos uno para que continúe con la búsqueda al recibir el siguiente string
	++i;
}

void SUFFIX_TREE::stringMatch(string& ss, string& T, vector<pair<pair<int, int>, string>>& delimiters)
{
	auto t_init = std::chrono::high_resolution_clock::now();
	vector<string> stopWordsList = loadStopWords();
	// primero formateamos la entrada
	string query_ss = formatString(ss, stopWordsList, alphabet);
	shared_ptr<SUFFIX_NODE> r = root_state, rp;
	size_t strlength = query_ss.length(), i = 0;
	int k, p;
	while (i < strlength)
	{
		// c = ss[i];
		// let g'(r, (k,p)) = r' be the t_c transition from r
		rp = r->getTransition(query_ss[i], k, p);
		if (!rp)// si r' no existe
		{
			cout << "Patron no encontrado\n";
			auto t_end = std::chrono::high_resolution_clock::now();
			std::cout << "busqueda realizada en " <<
				chrono::duration_cast<chrono::microseconds>(t_end - t_init).count() << " usec\n";
			return;
		}
		else
		{
			int j = k;
			while (j <= p && i < strlength && T[j] == query_ss[i])
			{
				++j;
				++i;
			}
			if (i == strlength)
			{
				map<string, int> results;
				getResults(rp, k, p, results, delimiters);

				auto t_end = std::chrono::high_resolution_clock::now();
				std::cout << "busqueda realizada en " <<
					chrono::duration_cast<chrono::microseconds>(t_end - t_init).count() << " usec\n";
				
				showResults(results, delimiters.size());
				return;
			}
		}
		r = rp;
	}
}

void SUFFIX_TREE::getResults(shared_ptr<SUFFIX_NODE>& r, int k, int p, map<string, int>& results,
	vector<pair<pair<int, int>, string>>& delimiters)
{
	if (r == nullptr) // esto me indica que es una hoja
	{
		results.insert(make_pair(getIndex(delimiters, k), 1));
		return; // solo hubo un resultado
	}
	stack<shared_ptr<SUFFIX_NODE>> dfs; // depth first search
	shared_ptr<SUFFIX_NODE> top;
	dfs.push(r); // al inicio dfs solo contiene r
	while (!dfs.empty())
	{
		top = dfs.top();
		dfs.pop();

		for (auto& it_trans : top->getAllTransitions()) {
			if (it_trans.second.second == nullptr)// esto me indica que el una hoja
				++results[getIndex(delimiters, it_trans.second.first.first)];
			else
				dfs.push(it_trans.second.second);
		}

	}
	return;
}


SUFFIX_TREE::~SUFFIX_TREE()
{
	//cout << "references to auxstate: " << aux_state.use_count() << endl;

	//cout << "references to rootstate: " << root_state.use_count() << endl;

}

#endif // !SUFFIX_TREE_V2_HPP_

