#include "UniqueIdentifier.h"

#include <algorithm>

UniqueIdentifier* UniqueIdentifier::instance_ = 0;
int UniqueIdentifier::index = 0;
std::vector<int> UniqueIdentifier::ids_ = std::vector<int>(UniqueIdentifier::range_, -1);


UniqueIdentifier* UniqueIdentifier::getInstance()
{
	if (instance_ == 0) {
		instance_ = new UniqueIdentifier;
		for (int i = 0; i < range_; ++i) {
			ids_[i] = i;
		}
		std::random_shuffle(ids_.begin(), ids_.end());
	}
	return instance_;
}


int UniqueIdentifier::id()
{
	if (index > ids_.size() - 1)
		index = 0;
	return ids_[index++];
}