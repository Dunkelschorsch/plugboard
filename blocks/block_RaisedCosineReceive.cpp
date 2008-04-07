#include "block/block.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"

#include <itpp/itcomm.h>
#include <itpp/itstat.h>
#include <itpp/signal/filter.h>


template< typename T1, typename T2 >
class Matched_Filter : public itpp::Filter< T1, T2, T1 >
{
public:
    Matched_Filter(itpp::Vec<T2> filter_coefficients_in, int downsample_factor_in) :
        filter_coefficients(filter_coefficients_in),
        downsample_factor(downsample_factor_in),
        filter_length(filter_coefficients.size()),
        memory_size(filter_length-1),
        shift(0)
    {
        memory.set_size( memory_size ) ;
        memory.zeros();
    }

    T1 filter(T1 const sample)
    {
        T1 output = sample * filter_coefficients(0);

        for(unsigned int coeff_num=1; coeff_num < filter_length; coeff_num++)
        {
            output += memory_shifted(coeff_num) * filter_coefficients(coeff_num);
        }

        // put first sample into filter memory
        put_sample(sample);

        return output;
    }

    itpp::Vec<T1> operator()(itpp::Vec<T1> const& v)
    {
        itpp::Vec<T1> output(v.size()/downsample_factor);
        for(int i=0; i<output.size(); i++)
        {
               output(i) = filter(v(i*downsample_factor));

               for(unsigned int j=1; j<downsample_factor; j++)
               {
                    put_sample(v(i*downsample_factor + j));
               }
        }
        return output;
    }

private:
    const itpp::Vec<T2> filter_coefficients;
    const unsigned int downsample_factor;
    const unsigned int filter_length;
    const unsigned int memory_size;
    itpp::Vec<T1> memory;

    unsigned int shift;

    inline T1& memory_shifted(unsigned int n)
    {
        return memory[ (shift - n + memory_size) % memory_size ];
    }

    void put_sample(T1 const sample)
    {
        memory[shift] = sample;

        if(++shift == memory_size)
            shift = 0;
    }
};



class HumpBlock : public Block, public Sink, public Source
{
public:
	HumpBlock();
	~HumpBlock();
private:
	void configure_parameters();
	void setup_input_ports();
	void setup_output_ports();

	void process();
	void initialize();

	// input signals
	const InPort* sig_in_;
	const itpp::cvec *in_vector_;

	// output signals
	OutPort* sig_out_;
	itpp::cvec *out_vector_;

	// block parameters
	int32_vec_t framesize_;
	real_vec_t Ts_;
	real_vec_t alpha_;
	int32_vec_t filter_length_;
	int32_vec_t downsampling_factor_;

	// pulse shaper object
	itpp::Root_Raised_Cosine< complex_t > *rc;
	// filter object
	Matched_Filter< complex_t, double > *mf;
};



HumpBlock::HumpBlock()
{
	set_name("RaisedCosineReceive");
	set_description("Raised cosine receive filter");
}


HumpBlock::~HumpBlock()
{
	delete mf;
}


void HumpBlock::setup_input_ports()
{
	sig_in_ = add_port(new InPort("in", complex, Ts_[0], framesize_[0]));
}



void HumpBlock::setup_output_ports()
{
	sig_out_ = add_port(new OutPort("out", complex,
		sig_in_->get_Ts() * downsampling_factor_[0],
		sig_in_->get_frame_size() / downsampling_factor_[0])
	);
}



void HumpBlock::initialize()
{
	in_vector_ = get_signal< complex_t >(sig_in_);
	out_vector_ = get_signal< complex_t >(sig_out_);

	framesize_[0] = sig_in_->get_frame_size();

	rc = new itpp::Root_Raised_Cosine< complex_t >(alpha_[0], filter_length_[0], downsampling_factor_[0]);
	mf = new Matched_Filter< complex_t, double >(rc->get_pulse_shape(), downsampling_factor_[0]);
	delete rc;
}



void HumpBlock::configure_parameters()
{
	add_parameter
	(
		(new Parameter(&Ts_, real, "Sample Time"))
		->add_constraint(new LessThanConstraint< real_t >(0.0, true))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&framesize_, int32, "Frame Size"))
		->add_constraint(new LessThanConstraint< int32_t >(0, true))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&alpha_, real, "Roll-off factor"))
		->add_constraint(new LessThanConstraint< real_t >(0.0, true))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&filter_length_, int32, "Filter length"))
		->add_constraint(new ModuloConstraint< int32_t >(2))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&downsampling_factor_, int32, "Downsampling factor"))
		->add_constraint(new GreaterThanConstraint< int32_t >(1))
		->add_constraint(new SizeConstraint(1))
	);
}



void HumpBlock::process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl << " samples in(" << in_vector_->size() << "): " ;
	std::cout << "rms: " << sqrt(itpp::mean(itpp::sqr(*in_vector_))) << " ";
	std::cout << *in_vector_ << std::endl;
#endif

        *out_vector_ = (*mf)(*in_vector_);

#ifndef NDEBUG
	std::cout << " samples out(" << out_vector_->size() << "): " ;
	std::cout << "rms: " << sqrt(itpp::mean(itpp::sqr(*out_vector_))) << " ";
	std::cout << *out_vector_ << std::endl;
#endif
}


#include "block/create.hpp"
