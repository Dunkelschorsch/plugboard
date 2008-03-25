#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <boost/pool/detail/singleton.hpp>
#include "pimpl/pimpl.h"


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

	bool threading_enabled();
	void set_threading(bool);

private:
	Environment();
};

#endif // ENVIRONMENT_HPP
