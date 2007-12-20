#ifndef _CONSOLE_HPP
#define _CONSOLE_HPP

#include <string>
class BlockLoader;
class System;


class HumpShell
{
struct HumpShellImpl;

public:
	HumpShell(System & s, BlockLoader & bl);
	~HumpShell();

	const std::string read_command();

	bool execute_command();

private:
	HumpShellImpl * d;
};

#endif // _CONSOLE_HPP
