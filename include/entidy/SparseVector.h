#pragma once

#include <cmath>
#include <memory>
#include <string>

#include <entidy/Exception.h>
#include <entidy/MemoryManager.h>

namespace entidy
{
#define DEFAULT_SV_SIZE 512

using namespace std;

template <size_t PageSize>
class SparseVectorImpl;
template <size_t PageSize>
using SparseVector = shared_ptr<SparseVectorImpl<PageSize>>;

template <size_t Size>
struct Page
{
	intptr_t data[Size];
	size_t count = 0;
};

template <size_t PageSize>
class SparseVectorImpl
{
protected:
	vector<Page<PageSize>*> pages;
	MemoryManager memory_manager;
	string key;
	size_t size = 0;

	Page<PageSize>* Pop()
	{
		Page<PageSize>* page = memory_manager->Pop<Page<PageSize>>(key);
		new(page) Page<PageSize>();
		return page;
	}

public:
	SparseVectorImpl(MemoryManager memory_manager)
	{
		this->memory_manager = memory_manager;
		this->key = "entidy::PagedVector(" + to_string(PageSize) + ")";
	}

	~SparseVectorImpl()
	{
		for(size_t i = 0; i < pages.size(); i++)
		{
			if(pages[i] != nullptr)
				memory_manager->Push(key, (intptr_t)pages[i]);
		}
	}

	intptr_t Read(size_t index) const
	{
		size_t page_index = std::floor(index / PageSize);
		if(page_index >= pages.size())
			return 0;
		if(pages[page_index] == nullptr)
			return 0;

		size_t block_index = index - (page_index * PageSize);
		return pages[page_index]->data[block_index];
	}

	void Write(size_t index, intptr_t value)
	{
		if(value == 0)
			return;

		size_t page_index = std::floor(index / PageSize);

		while(page_index >= pages.size())
			pages.push_back(nullptr);

		if(pages[page_index] == nullptr)
			pages[page_index] = Pop();

		size_t block_index = index - (page_index * PageSize);
		intptr_t prev = pages[page_index]->data[block_index];
		pages[page_index]->data[block_index] = value;

		if(prev == 0)
		{
			pages[page_index]->count++;
			++size;
		}
	}

	intptr_t Erase(size_t index)
	{
		size_t page_index = std::floor(index / PageSize);

		if(page_index >= pages.size())
			return 0;

		if(pages[page_index] == nullptr)
			return 0;

		size_t block_index = index - (page_index * PageSize);
		intptr_t prev = pages[page_index]->data[block_index];
		pages[page_index]->data[block_index] = 0;
		if(prev != 0)
		{
			pages[page_index]->count--;
			--size;
		}

		if(pages[page_index]->count == 0)
		{
			memory_manager->Push(key, (intptr_t)(pages[page_index]));
			pages[page_index] = nullptr;
		}

		return prev;
	}

	size_t Size() const
	{
		return size;
	}
};

} // namespace entidy
