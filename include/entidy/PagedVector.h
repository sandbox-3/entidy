#pragma once

#include <memory>
#include <string>
#include <cmath>

#include <entidy/Exception.h>
#include <entidy/MemoryManager.h>

namespace entidy
{
using namespace std;

class MemoryManagerImpl;
using MemoryManager = shared_ptr<MemoryManagerImpl>;

template <size_t Size>
struct Page
{
	intptr_t data[Size];
    size_t count;
};

template <size_t PageSize>
class PagedVector
{
protected:
	vector<Page<PageSize>*> pages;
	MemoryManager memory_manager;
    string key;

public:
	PagedVector(MemoryManager memory_manager)
    { 
        this->memory_manager = memory_manager;
        this->key = "entidy::PagedVector(" + to_string(PageSize) + ")";
    }

    ~PagedVector()
    {
        for(auto it : pages)
        {
            if(it != nullptr)
                memory_manager->Push(key, (intptr_t)it);
        }
    }

    const intptr_t Read(size_t index) const
    {
        size_t page_index = std::floor(index / PageSize);
        if(page_index >= pages.size())
            throw(EntidyException("PagedVector::operator[] Index [" + to_string(index) + "] out of range (" + to_string(pages.size() * PageSize) + ")"));

        size_t block_index = index - (page_index * PageSize);
        return pages[page_index]->data[block_index];
    }
    
    void Write (size_t index, intptr_t value)
    {
        size_t page_index = std::floor(index / PageSize);
        while(page_index >= pages.size())
            pages.push_back(nullptr);
        
        if(pages[page_index] == nullptr)
            pages[page_index] = memory_manager->Pop<Page<PageSize>>(key);
        
        size_t block_index = index - (page_index * PageSize);

        intptr_t prev = pages[page_index]->data[block_index];
        pages[page_index]->data[block_index] = value;

        if(prev == 0 && value != 0)
            pages[page_index]->count++;
        else if(value == 0 && prev != 0)
            pages[page_index]->count--;
        
        if(pages[page_index]->count == 0)
        {
            memory_manager->Push(key, (intptr_t)pages[page_index]);
            pages[page_index] = nullptr;
        }
    }

    void Erase(size_t index)
    {
        Write(index, 0);
    }
    
};

} // namespace entidy