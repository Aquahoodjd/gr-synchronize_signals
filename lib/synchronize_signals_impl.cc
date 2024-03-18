/* -*- c++ -*- */
/*
 * Copyright 2024 Witold Duda.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "synchronize_signals_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace synchronize_signals_module {

using input_type = gr_complex;
using output_type = gr_complex;

synchronize_signals::sptr synchronize_signals::make(size_t itemsize, bool synchronize)
{
    return gnuradio::make_block_sptr<synchronize_signals_impl>(itemsize, synchronize);
}

synchronize_signals_impl::synchronize_signals_impl(size_t itemsize, bool synchronize)
    : gr::sync_block("synchronize_signals",
                     gr::io_signature::make(
                         2 /* min inputs */, 2 /* max inputs */, itemsize),
                     gr::io_signature::make(
                         2 /* min outputs */, 2 /*max outputs */, itemsize)),
        d_synchronize(synchronize),
        d_synchronize_state(false),
        i(0)
{
}

synchronize_signals_impl::~synchronize_signals_impl() {}

int synchronize_signals_impl::work(int noutput_items,
                                   gr_vector_const_void_star& input_items,
                                   gr_vector_void_star& output_items)
{
    if (d_synchronize != d_synchronize_state) {
        d_synchronize_state = d_synchronize;
        std::cout << "Change" << std::endl;
    }
    if (d_synchronize == true){
        const input_type* in1 = static_cast<const input_type*>(input_items[0]);
        const input_type* in2 = static_cast<const input_type*>(input_items[1]);

        output_type* out1 = static_cast<output_type*>(output_items[0]);
        std::copy(in1, in1 + noutput_items, out1);

        output_type* out2 = static_cast<output_type*>(output_items[1]);
        std::copy(in2, in2 + noutput_items, out2);
    }else{
        const input_type* in1 = static_cast<const input_type*>(input_items[0]);
        output_type* out1 = static_cast<output_type*>(output_items[0]);
        std::copy(in1, in1 + noutput_items, out1);

        const input_type* in2 = static_cast<const input_type*>(input_items[1]);
        output_type* out2 = static_cast<output_type*>(output_items[1]);
        std::copy(in2, in2 + noutput_items, out2);
    }



    return noutput_items;
}

} /* namespace synchronize_signals_module */
} /* namespace gr */
