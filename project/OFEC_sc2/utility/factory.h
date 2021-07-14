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
* class factory is for the registration of all problems and algorithms
*
*********************************************************************************/

#ifndef OFEC_FACTORY_H
#define OFEC_FACTORY_H

#include <map>
#include <set>
#include <string>
#include <functional>
#include "myexcept.h"
#include "../core/definition.h"

namespace OFEC {
	template<typename Base>
	struct factory {
		template<typename T>
		struct register_ {
			register_(const std::string& key, std::set<ProTag>&& tag) {
				map_.emplace(
					key,
					make_pair([]() {
						return new T();
						},
						std::forward<std::set<ProTag>>(tag))
				);
			}
		};

		static Base* produce(const std::string& key) {
			auto it = map_.find(key);
			if (it == map_.end())
				throw MyExcept("the key is not exist!");
			return it->second.first();
		}

		static const std::map<std::string, std::pair<std::function<Base* ()>, std::set<ProTag>> >& get() {
			return map_;
		}

		factory() = default;
	private:
		factory& operator=(const factory&) = delete;
		factory& operator=(factory&&) = delete;
		factory(const factory&) = delete;
		factory(factory&&) = delete;
		static std::map<std::string, std::pair<std::function<Base* ()>, std::set<ProTag>>> map_;
	};

	template<typename Base>
	std::map<std::string, std::pair<std::function<Base* ()>, std::set<ProTag>> > factory<Base>::map_;

#define REGISTER(Base, Derived, key, tag) factory<Base>::register_<Derived> reg_##Derived(key, tag)
}

#endif
