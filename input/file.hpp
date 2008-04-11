#ifndef FILE_HPP
#define FILE_HPP

#include <string>

namespace plugboard
{
	class BlockLoader;
	class System;

	class FileInput
	{
	struct FileInputImpl;

	public:
		FileInput();
		~FileInput();

		bool execute_command(const std::string& file_name);

	private:
		FileInputImpl * d;
	};

} // namespace plugboard

#endif // FILE_HPP
