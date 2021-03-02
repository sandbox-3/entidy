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
	size_t size = 0;

	/**
     * @brief Returns a recycled or created page from the memory pool.
     * @return A pointer to the created page.
     */
	Page<PageSize>* Pop()
	{
		Page<PageSize>* page = memory_manager->Pop<Page<PageSize>>();
		new(page) Page<PageSize>();
		return page;
	}

public:
	SparseVectorImpl(MemoryManager memory_manager)
	{
		this->memory_manager = memory_manager;
	}

	/**
     * @brief Returns all the pages to the memory pool for recycling.
     */
	~SparseVectorImpl()
	{
		for(size_t i = 0; i < pages.size(); i++)
		{
			if(pages[i] != nullptr)
				memory_manager->Push((intptr_t)pages[i]);
		}
	}

	/**
     * @brief Returns value at selected index. If page does not exist, returns 0
     * @return Value at selected index or 0 if empty.
     */
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

	/**
     * @brief Writes or replaces value 'value' at index 'index'. Creates page if it doesn't exist.
     * If 'value' is 0, no page is created.
     * @return Previous value if cell at 'index' was not 0.
     */
	intptr_t Write(size_t index, intptr_t value)
	{
		if(value == 0)
			return 0;

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

		return prev;
	}

	/**
     * @brief Sets value at index 'index' to 0.
     * If page is empty after erasure, sends page back to memory pool for recycling.
     * @return Previous value at 'index'.
     */
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
			memory_manager->Push((intptr_t)(pages[page_index]));
			pages[page_index] = nullptr;
		}

		return prev;
	}

	/**
     * @brief Returns total number of non-zero items in SparseVector.
     * @return Total number of non-zero items in SparseVector.
     */
	size_t Size() const
	{
		return size;
	}
};

} // namespace entidy
