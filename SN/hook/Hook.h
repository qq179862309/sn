#pragma once
class CHook
{
public:
	CHook();
	virtual ~CHook();
public:
	int				hookWindowAPI(tstring strModule, char* funcName, DWORD dwFunction);
};

