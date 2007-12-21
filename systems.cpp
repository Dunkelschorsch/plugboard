#include "systems.hpp"
#include "system.hpp"
#include "subsystem.hpp"



struct DeleteAction
{
	template< typename PairT >
	void operator()(PairT& e) const
	{
		delete e.second;
	}
};



Systems::Systems() : root_sys_name_("__root__")
{
	sys_map_[root_sys_name_] = new System;
}



Systems::~Systems()
{
	for_each
	(
		sys_map_.begin(),
		sys_map_.end(),
		DeleteAction()
	);
}



Systems & Systems::instance( )
{
	Systems & s = singleton::instance();
	return s;
}



void Systems::add_subsystem(const std::string& subsys_name, Subsystem * const subsys)
{
	if(sys_map_.find(subsys_name) != sys_map_.end())
	{
		throw("up");
		//TODO come up with a proper exception
	}
	sys_map_.insert(std::make_pair(subsys_name, subsys));
}



Subsystem * Systems::get_subsystem( const std::string & subsys_name ) const
{
	if(sys_map_.find(subsys_name) == sys_map_.end())
	{
		throw("up");
		//TODO come up with a proper exception
	}
	return dynamic_cast< Subsystem* >(sys_map_.find(subsys_name)->second);
}



Systems::operator System*()
{
	return sys_map_[root_sys_name_];
}



System * Systems::get_root( ) const
{
	return sys_map_.find(root_sys_name_)->second;
}
