
/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* some general methods used in OFEC are defined in this file, including distance
* metrics, solution domination relationship, 
*
*********************************************************************************/

#ifndef OFEC_FINCTIONAL_H
#define OFEC_FINCTIONAL_H

#include <cmath>
#include <vector>
#include "../core/definition.h"
#include <cstdio>
#include <string>
#include <functional>

namespace OFEC {	
		/*
		distance measures between two std::vector-based points
		*/
	template<typename Iter1, typename Iter2>
	Real euclidean_distance(Iter1 first1, Iter1 last1, Iter2 first2) {
		Real dis = 0;
		while (first1 != last1) {
			dis += (*first1 - *first2)*(*first1 - *first2);
			++first1;
			++first2;
		}
		dis = sqrt(dis);
		return dis;
	}

	template<typename Iter1, typename Iter2>
	Real manhattan_distance(Iter1 first1, Iter1 last1, Iter2 first2) {
		Real dis = 0;
		while (first1 != last1) {
			dis += fabs(*first1++ - *first2++);
		}
		return dis;
	}
	


	template<typename Iter1, typename Iter2>
	int hamming_distance(Iter1 first1, Iter1 last1, Iter2 first2)  {
		int dis = 0;
		while (first1 != last1) {
			dis += *first1++ != *first2++;
		}
		return dis;
	}


	//domination relationship between two objective vectors
	template<typename T = Real >
	Dominance objectiveCompare(const std::vector<T>& a, const std::vector<T>& b, const std::vector<OptMode> &mode)  {
		if (a.size() != b.size()) 
			return Dominance::NonComparable;

		int better = 0, worse = 0, equal = 0;
		for (decltype(a.size()) i = 0; i<a.size(); ++i) {
			if (mode[i] == OptMode::Minimize) {
				if (a[i] < b[i]) {
					if (worse > 0)
						return Dominance::NonDominated;
					else
						++better;
				}
				else if (a[i] > b[i]) {
					if (better > 0)
						return Dominance::NonDominated;
					else
						++worse;
				}
				else {
					++equal;
				}
			}
			else {
				if (a[i] > b[i]) {
					if (worse > 0)
						return Dominance::NonDominated;
					else
						++better;
				}
				else if (a[i] < b[i]) {
					if (better > 0)
						return Dominance::NonDominated;
					else
						++worse;
				}
				else {
					++equal;
				}
			}
		}
		if (equal == a.size()) return Dominance::Equal;
		else if (worse == 0) return Dominance::Dominant;
		else if (better == 0) return Dominance::Dominated;
        else return Dominance::NonDominated;
	}

	template<typename T = Real >
	Dominance objectiveCompare(const std::vector<T> & a, const std::vector<T> & b, OptMode mode) {
		if (a.size() != b.size())
			return Dominance::NonComparable;

		int better = 0, worse = 0, equal = 0;
		for (decltype(a.size()) i = 0; i < a.size(); ++i) {
			if (mode == OptMode::Minimize) {
				if (a[i] < b[i]) {
					if (worse > 0)
						return Dominance::NonDominated;
					else
						++better;
				}
				else if (a[i] > b[i]) {
					if (better > 0)
						return Dominance::NonDominated;
					else
						++worse;
				}
				else {
					++equal;
				}
			}
			else {
				if (a[i] > b[i]) {
					if (worse > 0)
						return Dominance::NonDominated;
					else
						++better;
				}
				else if (a[i] < b[i]) {
					if (better > 0)
						return Dominance::NonDominated;
					else
						++worse;
				}
				else {
					++equal;
				}
			}
		}
		if (equal == a.size()) return Dominance::Equal;
		else if (worse == 0) return Dominance::Dominant;
		else if (better == 0) return Dominance::Dominated;
        else return Dominance::NonDominated;
	}

	//add by tanqingshan. return the first rank points.
	template <typename T=Real>
	std::vector<std::vector<T>*> & get_nondominated_set(std::vector<std::vector<T>*> &data, OptMode opt_mode) {
		size_t n = data.size();
		std::vector<std::vector<T>*> obj = data;
		data.clear();
		data.push_back(obj[0]);
		for (size_t i = 1; i < n; i++) {
			size_t m = data.size();
			for (size_t j = 0; j < m; j++) {
				Dominance temp_ = objectiveCompare(*obj[i], *data[j], opt_mode);
				if (temp_ == Dominance::Equal) {
					data.push_back(obj[i]);
				}
				else if (temp_ == Dominance::Dominant) {
					data[j] = obj[i];
					break;
				}
				else if (temp_ == Dominance::NonDominated) {
					if (j == m - 1)
						data.push_back(obj[i]);
					else
						continue;
				}
				else {
					continue;
				}
			}
		}
		return data;
	}

	template <typename T>
	int sign(T val) {
		return (T(0) < val) - (val < T(0));
	}

	template<typename T>
	bool less(const T &d1, const T &d2, bool ascending) {
		if (ascending) {
			if (d1 < d2) return true;
			else return false;
		}
		else {
			if (d1 < d2) return false;
			else return true;
		}
	}

	template<typename T>
	bool less(T *d1, T *d2, bool ascending) {
		if (ascending) {
			if (*d1 < *d2) return true;
			else return false;
		}
		else {
			if (*d1 < *d2) return false;
			else return true;
		}
	}

	namespace merge_sort_func {
		template<typename T>
		void merge(const T& data, std::vector<int>& A, std::vector<int>& B, int l, int m, int r, bool ascending) {
			int i = l, j = m;
			for (int k = l; k < r; ++k) {
				if (i < m && (j >= r || less(data[A[i]], data[A[j]], ascending))) {
					B[k] = A[i];
					i++;
				}
				else {
					B[k] = A[j];
					j++;
				}
			}
		}

		template<typename T>
		void sort(const T& data, std::vector<int>& B, std::vector<int>& A, int l, int r, bool ascending){
			if (r - l < 2)
				return;
			int m = (r + l) / 2;
			sort(data, A, B, l, m, ascending);
			sort(data, A, B, m, r, ascending);
			merge(data, B, A, l, m, r, ascending);
		}
	}

	/* index[i] denotes the index of the i-th minimum (in ascending order) element */
	template<typename T>
	void merge_sort(const T &data, int size, std::vector<int>& index, bool ascending = true, int low = 0, int up = -1, int num = -1, bool start = true) {
		if (start) {
			if (up == -1) up = size - 1;
			if (num == -1) num = size;
			if (index.size() == 0 || index.size() != size)		index.resize(size);
			for (auto i = index.begin(); i != index.end(); ++i) *i = i - index.begin();
		}
		if (low >= up) return;

		std::vector<int> B(index);
		merge_sort_func::sort(data, B, index, low, up + 1, ascending);
	}

	template <class T>
	std::vector<int> amend_order(const T &data, const std::vector<int> &index) {
		/*amend index in the case of the same item values in data. Note: data must be
		sorted with results stored in index, e.g.,
		data[]=[5,2,2,1]; index[]=[3,2,1,0]; //ascending order
		after amendation  index[]=[2,1,1,0];
		*/
		std::vector<int> result(index.size());
		for (int r = 1, idx = 1; r <= index.size(); r++, idx++) {
			int temp = r, count = 1;
			while (temp<index.size() &&data[index[temp - 1]] == data[index[temp]]) {
				count++; 
				temp++; 
			}
			for (int k = 0; k<count; k++) result[index[r + k - 1]] = idx;
			r += count - 1;
		}
		return result;
	}
	template<typename T>
	inline T chaotic_value(T x, T min, T max, T rChaoticConstant = 3.54) {
		// return a value calculated by logistic map
		if (min>max) return -1;
		T val;
		val = (x - min) / (max - min);
		val = rChaoticConstant*val*(1 - val);
		return min + val*(max - min);
	}

	inline bool exists(const std::string& name) {
		if (FILE *file = fopen(name.c_str(), "r")) {
			fclose(file);
			return true;
		}
		else {
			return false;
		}
	}
	template<typename T>
	inline T map_real(T value, T input_min, T input_max, T output_min, T output_max) {
		return ((value - input_min) / (input_max - input_min) * (output_max - output_min) + output_min);
	}
	template<typename T,typename K>
	inline void calMeanAndStd(const std::vector<T> &data, K&mean, K&std) {
		mean = 0;
		for (T val : data)
			mean += val;
		mean /= static_cast<K>(data.size());
		std = 0;
		for (T val : data)
			std += (val - mean) * (val - mean);
		std = sqrt(std / static_cast<K>(data.size()));
	}

	template<typename Individual>
	inline std::vector<int> calBestIdx(const std::vector<Individual>& inds, std::function<Dominance(const Individual&, const Individual&)>& comp) {

		std::vector<int> best_idx;
		std::vector<bool> flag(inds.size(), true);

		for (size_t j = 0; j < inds.size(); j++) {
			for (size_t i = 0; i < inds.size(); i++) {
				if (i == j || !flag[j] || !flag[i]) continue;
				if (comp(inds[i], inds[j])) {
					flag[i] = false;
				}
			}
		}
		for (size_t i = 0; i < inds.size(); i++) {
			if (flag[i])
				best_idx.push_back(i);
		}

		return std::move(best_idx);
	}
}
#endif // !OFEC_FINCTIONAL_H

