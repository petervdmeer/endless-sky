/* ConditionsProvider.cpp
Copyright (c) 2020 by Peter van der Meer

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "ConditionsProvider.h"

using namespace std;



// Retrieve a sum (addition) of all "condition" flags starting with the
// given prefix. Actual implementers of this interface can choose to override
// this method and provide a more efficient implementation.
int64_t ConditionsProvider::GetConditionSum(const string &prefix) const
{
	int64_t returnValue = 0;
	map<string, int64_t> summationMap;
	GetConditions(summationMap, prefix);
	for(auto &it : summationMap)
		returnValue += it.second;
	return returnValue;
}



// Add a value to a condition. Returns true on success, false on failure.
// Actual implementers of this interface can choose to override this method
// and provide a more efficient implementation.
bool ConditionsProvider::AddCondition(const string &name, int64_t value)
{
	return SetCondition(name, GetCondition(name) + value);
}



// Set a value for a condition. The default implementation returns false
// to indicate a read-only variable. Implementers of this interface can
// override if they support variable modifications.
bool ConditionsProvider::SetCondition(const string &name, int64_t value)
{
	return false;
}



// Erase a condition completely. The default implementation returns false
// to indicate a read-only variable. Implementers of this interface can
// override if they support variable modifications.
bool ConditionsProvider::EraseCondition(const string &name)
{
	return false;
}
