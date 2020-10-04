/* ConditionsProvider.h
Copyright (c) 2020 by Peter van der Meer

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef CONDITIONS_PROVIDER_H_
#define CONDITIONS_PROVIDER_H_

#include <iterator>
#include <map>
#include <string>



// Class that describes the interface to retrieve and store condition-variables.
// This class is to be inherited from by any class that provides condition-
// variables.
class ConditionsProvider {
public:
	class Iterator {
	public:
		// Constructor to construct from other iterator.
		Iterator(const Iterator& other);
		// Pre-increment; increase, then return(increased value, ref to self).
		Iterator& operator++();
		// Post-increment; store, increase, then return stored value.
		Iterator operator++(int);
		// Comparison operators.
		bool operator==(const Iterator& other) const;
		bool operator!=(const Iterator& other) const;
	};
	
	// Iterator helper functions.
	Iterator begin();
	Iterator end();
	
	// Retrieve a "condition" flag from this provider.
	virtual int64_t GetCondition(const std::string &name) const = 0;
	// Copy conditions matching the prefix into the given map.
	virtual void GetConditions(std::map<std::string, int64_t> &targetMap, const std::string &prefix) const = 0;
	// Retrieve a sum (addition) of all "condition" flags starting with the
	// given prefix.
	virtual int64_t GetConditionSum(const std::string &prefix) const;
	// Add a value to a condition, set a value for a condition or erase a
	// condition completely. Returns true on success, false on failure.
	virtual bool AddCondition(const std::string &name, int64_t value);
	virtual bool SetCondition(const std::string &name, int64_t value);
	virtual bool EraseCondition(const std::string &name);
};



#endif
