/* ConditionsStore.cpp
Copyright (c) 2020 by Peter van der Meer

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "ConditionsStore.h"

using namespace std;



// Get mutable access to the local (non-forwarded) conditions of this store.
map<string, int64_t> &ConditionsStore::Locals()
{
	return conditions;
}



// Read-only access to the local (non-forwarded) conditions of this store.
const map<string, int64_t> &ConditionsStore::Locals() const
{
	return conditions;
}



void ConditionsStore::RegisterChild(ConditionsProvider &child, const vector<string> &matchPrefixes, const vector<string> &matchExacts)
{
	// Store the pointers to the children to forward to.
	for(auto &matchPrefix: matchPrefixes)
		this->matchPrefixes[matchPrefix] = &child;
	for(auto &matchExact: matchExacts)
		this->matchExacts[matchExact] = &child;
}



void ConditionsStore::DeRegisterChild(ConditionsProvider &child)
{
	ConditionsProvider *childPtr = &child;
	// Remove the child from all matchlist entries where it was listed.
	for(auto it = matchPrefixes.begin(); it != matchPrefixes.end(); it++)
		if(it->second == childPtr)
			it = matchPrefixes.erase(it);
	
	for(auto it = matchExacts.begin(); it != matchExacts.end(); it++)
		if(it->second == childPtr)
			it = matchExacts.erase(it);
}
