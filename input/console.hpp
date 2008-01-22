#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <string>
class BlockLoader;
class System;


class HumpShell
{
struct HumpShellImpl;

public:
	HumpShell(System & s);
	~HumpShell();

	const std::string read_command();

	bool execute_command();

private:
	HumpShellImpl * d;
};

#endif // CONSOLE_HPP
