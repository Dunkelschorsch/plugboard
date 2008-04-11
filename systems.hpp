#ifndef SYSTEMS_HPP
#define SYSTEMS_HPP

#include <boost/pool/detail/singleton.hpp>
#include <map>
#include <string>
#include "system.hpp"

namespace plugboard
{
	class Subsystem;

	class Systems
	{
	typedef boost::details::pool::singleton_default< Systems > singleton;
	friend class boost::details::pool::singleton_default< Systems >;

		Systems();
		~Systems();

		std::string root_sys_name_;

		std::map< const std::string, System* > sys_map_;

	public:
		static Systems& instance();

		operator System*();

		void add_subsystem(const std::string& subsys_name, Subsystem * const subsys);

		Subsystem * get_subsystem(const std::string& subsys_name) const;

		System * get_root() const;
	};
} // namespace plugboard

#endif // SYSTEMS_HPP
