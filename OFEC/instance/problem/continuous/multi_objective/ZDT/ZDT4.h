//Register ZDT4 "MOP_ZDT4" MOP,ConOP

/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.

*************************************************************************/
// Created: 31 December 2014
// Modified: 29 Mar 2018 by Junchen Wang (wangjunchen@cug.edu.cn)

#ifndef ZDT4_H
#define ZDT4_H

#include "ZDT.h"

namespace ofec {
	class ZDT4 : public ZDT {
	protected:
		void evaluateObjective(Real *x, std::vector<Real> &obj) override;
	};
}

#endif //ZDT4_H