#pragma once

#include "IFileLoader.h"
#include "ILogInterface.h"

class IProjectInterface
{
public:
	virtual ~IProjectInterface() {}

	virtual IFileLoader& FileLoader() = 0;
	virtual ILogInterface& LogInterface() = 0;
};
