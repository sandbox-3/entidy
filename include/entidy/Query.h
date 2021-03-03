#pragma once

#include <deque>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>

#include <entidy/Entidy.h>
#include <entidy/Indexer.h>
#include <entidy/QueryParser.h>
#include <entidy/View.h>

namespace entidy
{
using namespace std;

class Entidy;

class Query
{
protected:
	vector<string> select;
	Indexer indexer;

	Query(Indexer idxer, const initializer_list<string>& keys)
		: indexer(idxer)
		, select(keys)
	{ }

public:
	/**
     * @brief Executes the query and returns a view with lists of pointers to the selected components.
     * @param filter Query string used to filter the entities.
     * @return A View with lists of pointers to the requested components.
     * @throw EntidyException if the filter string has a syntax error or is empty.
     * @example Query String Examples:
     * Velocity & Position & !Stationary
     * ((Velocity & Position) & !(Flying | Swimming))
     */
	View Having(const string& filter)
	{
		return indexer->Fetch(select, filter);
	}

	friend Entidy;
};

} // namespace entidy
