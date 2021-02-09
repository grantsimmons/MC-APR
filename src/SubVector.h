#ifndef SUBVECTOR_H
#define SUBVECTOR_H

#include <list> //Need some sort of library for iterator types and size_t??

template <typename T>

class SubVector {
    public:

        using iterator = typename T::iterator;

        using reference = typename std::iterator_traits<iterator>::reference;

        SubVector(iterator first, iterator last)
            : first(first), last(last) { 
		} 

        reference operator[](std::size_t idx) {   
            return *std::next(first, idx);
        } 

        iterator begin() const { return first; }  

        iterator end() const { return last; }

		std::size_t size() {
            return (std::size_t) (last-first);
        }

    private:

        iterator first, last;

};
#endif
