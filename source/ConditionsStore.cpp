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



// Empty constructor, just results in an emtpy store.
ConditionsStore::ConditionsStore()
{
}



// Constructor where a number of initial manually-set values are set.
ConditionsStore::ConditionsStore(initializer_list<pair<string, int64_t>> initialConditions)
{
	for(auto it : initialConditions)
		SetCondition(it.first, it.second);
}



// Constructor where a number of initial manually-set values are set.
ConditionsStore::ConditionsStore(const map<string, int64_t> initialConditions)
{
	for(auto it : initialConditions)
		SetCondition(it.first, it.second);
}



int64_t ConditionsStore::operator [] (const std::string &name) const
{
	return GetCondition(name);
}



bool ConditionsStore::HasCondition(const std::string &name) const
{
	// Check for on-demand condition.
	const OnDemand* cp = GetOnDemandProvider(name);
	if(cp != nullptr)
		return cp->hasFun(name);
	
	// Handle from internal storage if no on-demand condition available.
	auto it = conditions.find(name);
	return it != conditions.end();
}



// Add a value to a condition. Returns true on success, false on failure.
bool ConditionsStore::AddCondition(const string &name, int64_t value)
{
	// This code performes 2 lookups of the condition, once for get and
	// once for set. This might be optimized to a single lookup in a
	// later version of the code when we add on-demand conditions.
	
	return SetCondition(name, GetCondition(name) + value);
}



// Get a value for a condition, first by trying the children and if
// that doesn't succeed then internally in the store
int64_t ConditionsStore::GetCondition(const string &name) const
{
	// Check for on-demand condition.
	const OnDemand* cp = GetOnDemandProvider(name);
	if(cp != nullptr)
		return cp->getFun(name);
	
	// Handle from internal storage if no on-demand condition available.
	auto it = conditions.find(name);
	if(it != conditions.end())
		return it->second;
	
	// Return the default value if nothing was found.
	return 0;
}



// Set a value for a condition, first by trying the children and if
// that doesn't succeed then internally in the store.
bool ConditionsStore::SetCondition(const string &name, int64_t value)
{
	// Check for on-demand condition.
	const OnDemand* cp = GetOnDemandProvider(name);
	if(cp != nullptr)
		return cp->setFun(name, value);

	// Store in internal storage if no on-demand condition available.
	conditions[name] = value;
	return true;
}



// Erase a condition completely, first by trying the children and if
// that doesn't succeed then internally in the store.
bool ConditionsStore::EraseCondition(const string &name)
{
	// Check for on-demand condition.
	const OnDemand* cp = GetOnDemandProvider(name);
	if(cp != nullptr)
		return cp->eraseFun(name);

	// Erase from in internal storage if no on-demand condition available.
	auto it = conditions.find(name);
	if(it != conditions.end())
		conditions.erase(it);
	
	// The condition was either erased at this point, or it was not present
	// when we started. In either case the condition got succesfully removed.
	return true;
}



// Read-only access to the local (non-child) conditions of this store.
const map<string, int64_t> &ConditionsStore::Locals() const
{
	return conditions;
}



void ConditionsStore::AddExactOnDemandCondition(const string &name, OnDemand conditionProvider)
{
	// TODO: Move matching values stored in internal storage to provider (or at least remove here, or just warn)?
	matchExacts[name] = conditionProvider;
}



void ConditionsStore::AddPrefixOnDemandCondition(const string &prefix, OnDemand conditionsProvider)
{
	// TODO: Move matching values stored in internal storage to provider (or at least remove here, or just warn)?
	matchPrefixes[prefix] = conditionsProvider;
}




const ConditionsStore::OnDemand* ConditionsStore::GetOnDemandProvider(const std::string &name) const
{
	// First check if we should set based on exact names.
	auto exIt = matchExacts.find(name);
	if(exIt != matchExacts.end())
		return &(exIt->second);

	// Then check if this is a known prefix.
	// The lower_bound function will typically end up beyond the required
	// entry, because the entries are prefixes, but we can still arrive on
	// the exact location in case of an exact match.
	if(matchPrefixes.empty())
		return nullptr;

	auto preIt = matchPrefixes.lower_bound(name);
	if(preIt == matchPrefixes.end())
		--preIt;
	else if(preIt->first.compare(0, preIt->first.length(), name))
		return &(preIt->second);
	else if(preIt == matchPrefixes.begin())
		return nullptr;
	else
		--preIt;

	// We should have a match for preIt at this point if there were any
	// prefix matches to be made.
	if(preIt->first.compare(0, preIt->first.length(), name))
		return &(preIt->second);

	return nullptr;
}
