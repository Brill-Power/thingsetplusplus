#include <iterator>

// https://stackoverflow.com/questions/3623082/flattening-iterator
// A forward iterator that "flattens" a container of containers.  For example,
// a vector<vector<int>> containing { { 1, 2, 3 }, { 4, 5, 6 } } is iterated as
// a single range, { 1, 2, 3, 4, 5, 6 }.
template <typename OuterIterator> class FlatteningIterator
{
public:
    typedef OuterIterator outer_iterator;
    // https://stackoverflow.com/questions/3967996/how-to-retrieve-value-type-from-iterator-in-c
    typedef typename std::iterator_traits<OuterIterator>::value_type::iterator inner_iterator;

    typedef typename inner_iterator::value_type value_type;
    typedef typename inner_iterator::reference reference;

    FlatteningIterator()
    {}
    FlatteningIterator(outer_iterator it) : outer_it_(it), outer_end_(it)
    {}
    FlatteningIterator(outer_iterator it, outer_iterator end) : outer_it_(it), outer_end_(end)
    {
        if (outer_it_ == outer_end_) {
            return;
        }

        inner_it_ = outer_it_->begin();
        advance_past_empty_inner_containers();
    }

    reference operator*() const
    {
        return *inner_it_;
    }

    FlatteningIterator &operator++()
    {
        ++inner_it_;
        if (inner_it_ == outer_it_->end())
            advance_past_empty_inner_containers();
        return *this;
    }

    friend bool operator==(const FlatteningIterator &a, const FlatteningIterator &b)
    {
        if (a.outer_it_ != b.outer_it_)
            return false;

        if (a.outer_it_ != a.outer_end_ && b.outer_it_ != b.outer_end_ && a.inner_it_ != b.inner_it_)
            return false;

        return true;
    }

    friend bool operator!=(const FlatteningIterator &a, const FlatteningIterator &b)
    {
        return !(a == b);
    }

private:
    void advance_past_empty_inner_containers()
    {
        while (outer_it_ != outer_end_ && inner_it_ == outer_it_->end()) {
            ++outer_it_;
            if (outer_it_ != outer_end_)
                inner_it_ = outer_it_->begin();
        }
    }

    outer_iterator outer_it_;
    outer_iterator outer_end_;
    inner_iterator inner_it_;
};

template <typename Iterator> FlatteningIterator<Iterator> flatten(Iterator it)
{
    return FlatteningIterator<Iterator>(it, it);
}

template <typename Iterator> FlatteningIterator<Iterator> flatten(Iterator first, Iterator last)
{
    return FlatteningIterator<Iterator>(first, last);
}