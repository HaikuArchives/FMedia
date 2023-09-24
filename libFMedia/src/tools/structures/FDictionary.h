#ifndef F_FDictionary_H
#define F_FDictionary_H

#include "FContainer.h"

template <class kT, class T>
class FDictionary : public FContainer<T> {
	public:
		virtual T * Get(const kT & key) const = 0;
		virtual bool Put(const kT& key, const T& value) = 0;
};

#endif
