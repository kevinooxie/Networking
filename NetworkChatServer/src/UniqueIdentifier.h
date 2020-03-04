#pragma once

#include <vector>

class UniqueIdentifier
{
public:
	~UniqueIdentifier() {}

	static UniqueIdentifier* getInstance();

	static int id();

private:
	UniqueIdentifier() {}

	static std::vector<int> ids_;
	const static int range_ = 10000;  // cover up to 10000 clients connected to the server
	static int index;

	static UniqueIdentifier* instance_;
};
