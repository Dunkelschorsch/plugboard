#ifndef _CONSOLE_HPP
#define _CONSOLE_HPP

#include <string>
class BlockLoader;
class System;


class HumpFile
{
struct HumpFileImpl;

public:
	HumpFile();
	~HumpFile();

	bool execute_command(const std::string& file_name);

private:
	HumpFileImpl * d;
};

#endif // _CONSOLE_HPP
