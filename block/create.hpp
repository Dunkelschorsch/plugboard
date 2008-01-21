#ifndef BLOCK_CREATE_HPP
#define BLOCK_CREATE_HPP

#define ACCESS_FUNCS(NAME)	\
extern "C" {			\
Block* create()			\
{				\
	return new Block_##NAME;\
}				\
const string_t name()		\
{				\
	return #NAME;		\
} }

#endif // BLOCK_CREATE_HPP
