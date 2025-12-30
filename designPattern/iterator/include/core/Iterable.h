#pragma once
#include <memory>
#include "Iterator.h"
using namespace std;


template <typename T>
class Iterable{
    public:
    virtual unique_ptr<Iterator<T>> getIterator()=0;
    virtual ~Iterable()=default;
};