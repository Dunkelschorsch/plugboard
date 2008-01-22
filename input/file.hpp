#ifndef FILE_HPP
#define FILE_HPP

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

#endif // FILE_HPP
