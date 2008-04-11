#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <string>
#include <boost/any.hpp>
#include <boost/pool/detail/singleton.hpp>
#include "pimpl/pimpl.h"

namespace plugboard
{
	class Environment : public pimpl< Environment >::pointer_semantics
	{
	typedef boost::details::pool::singleton_default< Environment > singleton;
	friend class boost::details::pool::singleton_default< Environment >;

	public:
		static Environment& instance()
		{
			Environment& self = singleton::instance();
			return self;
		}

		const boost::any& get(const std::string&);

		Environment& set(const std::string&, boost::any);

	private:
		Environment();
	};
} // namespace plugboard

#endif // ENVIRONMENT_HPP
