#include "block/block.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"

#include <iostream>
#include <cstdio>



class HumpBlock : public Block, public Source, public Sink
{
public:

	HumpBlock();
	~HumpBlock();

	void initialize();
	void process();

private:
	void setup_input_ports();
	void setup_output_ports();
	void configure_parameters();

	template< typename T >
	void do_init();

	template< typename T >
	void do_work();

	template< typename T >
	void do_add();

	template< typename T >
	void do_mult();

	template< typename T >
	void do_sub();

	template< typename T >
	void do_div();

	OutPort *sig_out_;
	const InPort **sig_in_;

	void *v_out_;
	const void **v_in_;

	int32_vec_t num_inputs_;
	type_t input_type_;
	std::vector< std::string > op_;

	typedef enum { ADD, MUL, SUB, DIV } ops;
	ops operation_;
};


void HumpBlock::configure_parameters( )
{
	add_parameter
	(
		(new Parameter(&num_inputs_, int32, "Number of inputs"))
		->add_constraint(new GreaterThanConstraint< int32_t >(0))
		->add_constraint(new LessThanConstraint< int32_t >(1000))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&op_, string, "Operation"))
		->add_constraint(new SizeConstraint(1))
	);
}


void HumpBlock::setup_output_ports()
{
	/* calls to "add_port(InPort &) go here */
	sig_out_ = add_port(new OutPort("result", sig_in_[0]->get_type(), sig_in_[0]->get_Ts(), sig_in_[0]->get_frame_size()));
}


void HumpBlock::setup_input_ports()
{
	// only reserve that memory once
	if(num_input_ports_ != num_inputs_[0])
		sig_in_ = new const InPort* [num_inputs_[0]];

	for(int32_t i=0; i<num_inputs_[0]; ++i)
	{
		char portname[5];
		std::sprintf(portname, "in%d", i+1);

		sig_in_[i] = add_port(new InPort(portname));
	}
}


template< typename T >
void HumpBlock::do_init()
{
	v_out_ = get_signal< T >(sig_out_);
	for(int32_t i=0; i<num_inputs_[0]; ++i)
		v_in_[i] = get_signal< T >(sig_in_[i]);
}


void HumpBlock::initialize( )
{
	if(op_[0] == "+")
		operation_ = ADD;
	else if(op_[0] == "-")
		operation_ = SUB;
	else if(op_[0] == "*")
		operation_ = MUL;
	else if(op_[0] == "/")
		operation_ = DIV;

	v_in_ = new const void* [num_inputs_[0]];

	input_type_ = sig_in_[0]->get_type();

	if(input_type_ == int32)
		do_init< int32_t >();
	if(input_type_ == real)
		do_init< real_t >();
	if(input_type_ == complex)
		do_init< complex_t >();
}





template< typename T >
void HumpBlock::do_add()
{
	for(int32_t i=1; i<num_inputs_[0]; ++i)
	{
#ifndef NDEBUG
		std::cout << " in" << i+1 << ": " << *static_cast< const itpp::Vec<T>* >(v_in_[i]) << std::endl;
#endif
		*static_cast< itpp::Vec<T>* >(v_out_) +=
			*static_cast< const itpp::Vec<T>* >(v_in_[i]);
	}
}


template< typename T >
void HumpBlock::do_sub()
{
	for(int32_t i=1; i<num_inputs_[0]; ++i)
	{
#ifndef NDEBUG
		std::cout << " in" << i+1 << ": " << *static_cast< const itpp::Vec<T>* >(v_in_[i]) << std::endl;
#endif
		*static_cast< itpp::Vec<T>* >(v_out_) -=
			*static_cast< const itpp::Vec<T>* >(v_in_[i]);
	}
}


template< typename T >
void HumpBlock::do_mult()
{
	for(int32_t i=1; i<num_inputs_[0]; ++i)
	{
#ifndef NDEBUG
		std::cout << " in" << i+1 << ": " << *static_cast< const itpp::Vec<T>* >(v_in_[i]) << std::endl;
#endif
		*static_cast< itpp::Vec<T>* >(v_out_) =
			elem_mult(*static_cast< itpp::Vec<T>* >(v_out_), *static_cast< const itpp::Vec<T>* >(v_in_[i]));
	}
}


template< typename T >
void HumpBlock::do_div()
{
	for(int32_t i=1; i<num_inputs_[0]; ++i)
	{
#ifndef NDEBUG
		std::cout << " in" << i+1 << ": " << *static_cast< const itpp::Vec<T>* >(v_in_[i]) << std::endl;
#endif
		*static_cast< itpp::Vec<T>* >(v_out_) =
			elem_div(*static_cast< itpp::Vec<T>* >(v_out_), *static_cast< const itpp::Vec<T>* >(v_in_[i]));
	}
}


template< typename T >
void HumpBlock::do_work()
{
#ifndef NDEBUG
	std::cout << " in1: " << *static_cast< const itpp::Vec<T>* >(v_in_[0]) << std::endl;
#endif
	*static_cast< itpp::Vec<T>* >(v_out_) = *static_cast< const itpp::Vec<T>* >(v_in_[0]);
	
	if(operation_ == ADD)
		do_add< T >();
	else if(operation_ == SUB)
		do_sub< T >();
	else if(operation_ == MUL)
		do_mult< T >();
	else if(operation_ == DIV)
		do_div< T >();

#ifndef NDEBUG
	std::cout << " out: " << *static_cast< itpp::Vec<T>* >(v_out_) << std::endl;
#endif

}


void HumpBlock::process()
{
#ifndef NDEBUG
	std::cout << this->get_name_sys() << std::endl;
#endif
	if(input_type_ == int32)
		do_work< int32_t >();
	else if(input_type_ == real)
		do_work< real_t >();
	else
		do_work< complex_t >();
}


HumpBlock::HumpBlock()
{
	set_name("Add");
	set_description("Add an rbitrary, user-defined number of inputs.");
	configure_parameters();
}


HumpBlock::~HumpBlock()
{
#ifndef NDEBUG
	std::cout << "Bye from Block_" << get_name() << "!" << std::endl;
#endif
	if(is_initialized())
		delete[] v_in_;
	if(is_configured())
		delete[] sig_in_;
}


#include "block/create.hpp"
