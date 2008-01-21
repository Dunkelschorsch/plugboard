#include <cassert>
#include "grammar/variable/actions.hpp"
#include "types/base.hpp"


ValueAppendAction::ValueAppendAction(const variable_actor& v) : v_(v) { }


ValueAppendAction::result_type
ValueAppendAction::operator()(const variable_value_t& val_curr) const
{
#ifndef NDEBUG
    std::cout << "current value: " << val_curr << std::endl;
#endif
    if(val_curr.imag() == 0)
    {
#ifndef NDEBUG
        std::cout << "variable is not complex." << std::endl;
#endif
        if(static_cast< int32_t >(val_curr.real()) == val_curr.real())
        {
#ifndef NDEBUG
            std::cout << "variable is integer valued." << std::endl;
#endif
            v_().save_type_change(int32);
            v_().push_back(static_cast< int32_t >(val_curr.real()));
        }
        else
        {
#ifndef NDEBUG
            std::cout << "variable is real valued." << std::endl;
#endif
            v_().save_type_change(real);
            v_().push_back(static_cast< real_t >(val_curr.real()));
        }
    }
    else
    {
#ifndef NDEBUG
        std::cout << "variable is complex." << std::endl;
#endif
        v_().save_type_change(complex);
        v_().push_back(val_curr);
    }
#ifndef NDEBUG
        std::cout << "type of variable: " << v_().get_type() << std::endl;
#endif
}



DimensionAddAction::DimensionAddAction(const variable_actor& v, size_t size) : v_(v), size_(size) { }


DimensionAddAction::result_type
DimensionAddAction::operator()(const iterator_t&, const iterator_t&) const
{
#ifndef NDEBUG
    std::cout << "BLOODY HELL!!" << std::endl;
    bool spirit_does_not_suck = false;
    assert(spirit_does_not_suck);
#endif
}


DimensionAddAction::result_type
DimensionAddAction::operator()(const complex_t&) const
{
    v_().add_dimension(size_);
}
