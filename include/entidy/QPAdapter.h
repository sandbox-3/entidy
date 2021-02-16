#pragma once

#include <deque>
#include <memory>
#include <string>
#include <unordered_map>

#include <entidy/Indexer.h>
#include <entidy/QueryParser.h>

namespace entidy
{
using namespace std;

class IndexerImpl;
using Indexer = shared_ptr<IndexerImpl>;

#ifdef ENTIDY_32_BIT
#	include <roaring.hh>
using namespace roaring;
using BitMap = CRoaring;
#else
#	include <roaring64map.hh>
using namespace roaring;
using BitMap = Roaring64Map;
#endif

} // namespace entidy