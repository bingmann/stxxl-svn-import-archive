/***************************************************************************
 *  algo/test_stable_ksort.cpp
 *
 *  Part of the STXXL. See http://stxxl.sourceforge.net
 *
 *  Copyright (C) 2003 Roman Dementiev <dementiev@mpi-sb.mpg.de>
 *
 *  Distributed under the Boost Software License, Version 1.0.
 *  (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 **************************************************************************/

//! \example algo/test_stable_ksort.cpp
//! This is an example of how to use \c stxxl::ksort() algorithm

#include <stxxl/mng>
#include <stxxl/stable_ksort>
#include <stxxl/ksort>
#include <stxxl/vector>


struct my_type
{
    typedef unsigned key_type;

    key_type _key;
    char _data[128 - sizeof(key_type)];
    key_type key() const
    {
        return _key;
    }

    my_type() { }
    my_type(key_type __key) : _key(__key) { }

    static my_type min_value()
    {
        return my_type((std::numeric_limits<key_type>::min)());
    }
    static my_type max_value()
    {
        return my_type((std::numeric_limits<key_type>::max)());
    }
};

bool operator < (const my_type & a, const my_type & b)
{
    return a.key() < b.key();
}

int main()
{
    unsigned memory_to_use = 32 * 1024 * 1024;
    typedef stxxl::vector<my_type> vector_type;
    const stxxl::int64 n_records = 2 * 32 * stxxl::int64(1024 * 1024) / sizeof(my_type);
    vector_type v(n_records);

    stxxl::random_number32 rnd;
    STXXL_MSG("Filling vector... " << rnd() << " " << rnd() << " " << rnd());
    for (vector_type::size_type i = 0; i < v.size(); i++)
        v[i]._key = (rnd() / 2) * 2;


    STXXL_MSG("Checking order...");
    STXXL_MSG(((stxxl::is_sorted(v.begin(), v.end())) ? "OK" : "WRONG"));

    STXXL_MSG("Sorting...");
    stxxl::stable_ksort(v.begin(), v.end(), memory_to_use);

    STXXL_MSG("Checking order...");
    STXXL_MSG(((stxxl::is_sorted(v.begin(), v.end())) ? "OK" : "WRONG"));


    return 0;
}
