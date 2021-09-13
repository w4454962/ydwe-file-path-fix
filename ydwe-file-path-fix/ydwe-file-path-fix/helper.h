#pragma once

#include <base/config.h>

class Helper
{
public:
	Helper();
	~Helper();


	void attach();

	void detach();

private:
	bool m_attach;


};


Helper& get_helper();
