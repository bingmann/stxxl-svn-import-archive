/***************************************************************************
 *  containers/test_ext_merger.cpp
 *
 *  Part of the STXXL. See http://stxxl.sourceforge.net
 *
 *  Copyright (C) 2003 Roman Dementiev <dementiev@mpi-sb.mpg.de>
 *  Copyright (C) 2009 Andreas Beckmann <beckmann@cs.uni-frankfurt.de>
 *
 *  Distributed under the Boost Software License, Version 1.0.
 *  (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 **************************************************************************/

#include <limits>
#include <iterator>
#include <stxxl/priority_queue>

using stxxl::priority_queue_local::ext_merger;
using stxxl::priority_queue_local::loser_tree;

typedef int my_type;
typedef stxxl::typed_block<4096, my_type> block_type;


struct dummy_merger
{
    int & cnt;
    dummy_merger(int & c) : cnt(c) { }
    template <class OutputIterator>
    void multi_merge(OutputIterator b, OutputIterator e)
    {
        while (b != e)
        {
            *b = cnt;
            ++b;
            ++cnt;
        }
    }
};

struct my_cmp : public std::greater<my_type>
{
    my_type min_value() const
    {
        return (std::numeric_limits<my_type>::max)();
    }
    my_type max_value() const
    {
        return (std::numeric_limits<my_type>::min)();
    }
};

my_type * make_sequence(dummy_merger & dummy, int l)
{
    my_type * seq = new my_type[l + 1]; // + sentinel
    dummy.multi_merge(seq, seq + l);
    seq[l] = my_cmp().min_value();      // sentinel
    return seq;
}

int main()
{
    stxxl::read_write_pool<block_type> pool(1, 2);
    int cnt = 0;
    dummy_merger dummy(cnt);
    std::vector<my_type> output(1024 * 3);

    ext_merger<block_type, my_cmp, 5> merger(&pool);
    merger.insert_segment(dummy, 1024 * 3);
    cnt = 20;
    merger.insert_segment(dummy, 1024 * 4);
    cnt = 10;
    merger.insert_segment(dummy, 1024 * 4);
    cnt = -100;
    merger.insert_segment(dummy, 1024 * 4);
    merger.insert_segment(dummy, 1024 * 4);
    merger.multi_merge(output.begin(), output.end());

    loser_tree<my_type, my_cmp, 8> loser;
    my_type * seq1 = make_sequence(dummy, 1024);
    cnt = 20;
    my_type * seq2 = make_sequence(dummy, 1024);
    cnt = 10;
    my_type * seq3 = make_sequence(dummy, 1024);
    cnt = -100;
    my_type * seq4 = make_sequence(dummy, 1024);
    my_type * out = new my_type[4 * 1024];
    loser.init();
    loser.insert_segment(seq1, 1024);
    loser.insert_segment(seq2, 1024);
    loser.insert_segment(seq3, 1024);
    loser.insert_segment(seq4, 1024);

    loser.multi_merge(out, out + 1024);
    std::copy(out, out + 1024, std::ostream_iterator<my_type>(std::cout, "\n"));

    delete[] out;
}
