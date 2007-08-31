#ifndef _EXCEPTIONS_HPP
#define _EXCEPTIONS_HPP

#include <stdexcept>

struct non_existant_block_error : public std::runtime_error
{
	non_existant_block_error(const std::string& arg) : std::runtime_error(arg) {}

	inline const char *which() { return what(); }
};

struct non_existant_port_error : public std::runtime_error
{
	non_existant_port_error(const std::string& arg) : std::runtime_error(arg) {}

	inline const char *which() { return what(); }
};

struct block_not_configured_error : public std::runtime_error
{
	block_not_configured_error(const std::string& arg) : std::runtime_error(arg) {}

	inline const char *which() { return what(); }
};


struct duplicate_block_name_error : public std::runtime_error
{
	duplicate_block_name_error(const std::string& arg) : std::runtime_error(arg) {}

	inline const char *which() { return what(); }
};


struct duplicate_port_name_error : public std::runtime_error
{
	duplicate_port_name_error(const std::string& arg) : std::runtime_error(arg) {}

	inline const char *which() { return what(); }
};

#endif
