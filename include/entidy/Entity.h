#pragma once

namespace entidy
{
    using namespace std;

#ifdef ENTIDY_32_BIT
    using Entity = uint32_t;
#else
    using Entity = uint64_t;
#endif


} // namespace entidy