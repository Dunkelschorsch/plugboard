/*!
 * \file
 * \brief
 * \author Armin Schmidt
 *
 * ----------------------------------------------------------------------------
 *
 * PlugBoard - A versatile communication simulation framework
 * Copyright (C) 2007-2009  Armin Schmidt
 *
 * This file is part of PlugBoard.
 *
 * PlugBoard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PlugBoard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PlugBoard.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ----------------------------------------------------------------------------
 */

%module plugboard

%{
#define SWIG_FILE_WITH_INIT

#include "plugboard.hpp"
#include "types/base.hpp"
#include "variable/variable.hpp"

#define PB_DEBUG_MESSAGE_COLOUR \033[01;32m
#define PB_DEBUG_MESSAGE_SOURCE configuration
#include "colour_debug.hpp"

#define BOOST_PP_FILL_VAR(z, I, _)	\
  case NUMPY_TYPE(I):			\
  fill_variable_from_array< CPP_TYPE(I) >(ptr_to_1[i], PyTuple_GetItem(ptr_to_input, i)); \
  break;
%}


%include "exception.i"
%include "std_string.i"
%include "numpy.i"
%fragment("NumPy_Fragments");

/* the C++ exceptions can directly be used to generate nice python error messages */
%exception {
  try {
    $action
  } catch (plugboard::Exception< std::string > const& e) {
      std::string err_msg(e.what());
      if(not e.get_id().empty())
      {
	err_msg += ": »" + e.get_id() + "«";
      }
      SWIG_exception(SWIG_RuntimeError, err_msg.c_str());
  }
}


%inline %{

  template< class T >
  void fill_variable_from_array(plugboard::Variable &v, PyObject *o)
  {
    int num_dims = array_numdims(o);
    unsigned long numel = 0;
    void *data = array_data(o);

    for(int i=0; i<num_dims; ++i) {
      numel += ((PyArrayObject*)o)->dimensions[i];
    }

    for(unsigned long i=0; i<numel; ++i)
    {
      v.push_back<T>(static_cast< T* >(data)[i]);
    }
  };

  template< >
  void fill_variable_from_array<std::string>(plugboard::Variable &v, PyObject *o)
  {
    unsigned long numel = 1;
    int num_dims = array_numdims(o);

    int new_obj = 0;
    PyArrayObject *new_array = NULL;

    new_array = obj_to_array_contiguous_allow_conversion((PyObject*)o, NPY_STRING, &new_obj);

    char* next_str_start = new_array->data;
    std::string string_curr;

    for(unsigned long i=0; i<numel; ++i)
    {
      /* string are NOT null-terminated and their length is quite well hidden */
      int next_str_len = new_array->dimensions[num_dims];
      string_curr = std::string(next_str_start, next_str_start + next_str_len);
      next_str_start += next_str_len;
    }

    for(unsigned long i=0; i<numel; ++i)
    {
        v.push_back(string_curr);
    }
  };

%}


%typemap(in) (plugboard::Variable* v, int n)
{
  $2 = PyTuple_Size($input);
  $1 = new plugboard::Variable[$2];

  plugboard::Variable* ptr_to_1 = $1;
  PyObject *ptr_to_input = $input;

  if(not PyTuple_Check($input))
  {
    PyErr_SetString(PyExc_ValueError, "Expected a tuple");
    return NULL;
  }

  for(int i=0; i<$2; ++i)
  {
    if(not is_array(PyTuple_GetItem(ptr_to_input, i)))
    {
      PyErr_SetString(PyExc_ValueError, "Expected a numpy array.");
      return NULL;
    }

    switch(array_type(PyTuple_GetItem(ptr_to_input, i)))
    {
      BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_FILL_VAR, _)
      #undef BOOST_PP_FILL_VAR
      default:
	assert(0);
    }
  }
}


%typemap(freearg) (plugboard::Variable* v, int n)
{
  PB_DEBUG_MESSAGE("freeing " << $2 << " variables")
  delete[] $1;
}


%feature("shadow") __add_block(const std::string& block_type, const std::string& block_name, plugboard::Variable* block_args, int n) %{
def __add_block(*args):
    $action
%}

%pythoncode %{
import numpy

def load_blocks(block_dir):
	__load_blocks(block_dir)
	return

def add_block(block_type, block_name, *block_arguments):
	arguments = list()
	for i in block_arguments:
		arguments.append(numpy.atleast_1d(i))

	__add_block(block_type, block_name, tuple(arguments))

	return

def connect(source_block_name, source_port_name, sink_block_name, sink_port_name):
	__connect_ports(source_block_name, source_port_name, sink_block_name, sink_port_name)
	return

def run(times):
	__run(times)
	return
%}


%init
%{
  import_array();
  plugboard::Environment::instance().set("verbose_plugin_load", false);
%}

%include "plugboard.hpp"

