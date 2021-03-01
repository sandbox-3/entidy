#pragma once

#include <deque>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>

#include <entidy/QueryParser.h>
#include <entidy/Indexer.h>
#include <entidy/View.h>
#include <entidy/Entidy.h>

namespace entidy
{
using namespace std;

class EntidyImpl;
using Entidy = shared_ptr<EntidyImpl>;

class Query
{
protected:
	vector<string> select;
	Indexer indexer;

	Query(Indexer indexer, const initializer_list<string>& keys)
	{
		this->indexer = indexer;
		this->select = keys;
	}

public:
	View Having(const string& filter)
	{
		return indexer->Fetch(select, filter);
	}

	friend EntidyImpl;
};

} // namespace entidy