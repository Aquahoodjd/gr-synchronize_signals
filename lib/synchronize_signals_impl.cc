/* -*- c++ -*- */
/*
 * Copyright 2024 Witold Duda.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "synchronize_signals_impl.h"
#include <gnuradio/io_signature.h>
#include <complex>
#include <vector>
#include <cmath>
#include <gnuradio/fft/fft.h>

namespace gr {
namespace synchronize_signals_module {

using 	fft_complex_fwd = gr::fft::fft< gr_complex, true >;
using 	fft_complex_rev = gr::fft::fft< gr_complex, false >;
using input_type = gr_complex;
using output_type = gr_complex;

const double PI = 3.14159265358979323846;

synchronize_signals::sptr synchronize_signals::make(size_t itemsize, bool synchronize, int fft_size)
{
    return gnuradio::make_block_sptr<synchronize_signals_impl>(itemsize, synchronize, fft_size);
}

synchronize_signals_impl::synchronize_signals_impl(size_t itemsize, bool synchronize, int fft_size)
    : gr::sync_block("synchronize_signals",
                     gr::io_signature::make(
                         2 /* min inputs */, 2 /* max inputs */, itemsize),
                     gr::io_signature::make(
                         2 /* min outputs */, 2 /*max outputs */, itemsize)),
        d_synchronize(synchronize),
        d_synchronize_state(false),
        i(0),
        d_fft_size(fft_size)
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

        fft_complex_fwd fft_fwd1(d_fft_size);
        fft_complex_fwd fft_fwd2(d_fft_size);
        fft_complex_rev fft_rev(d_fft_size);

        // Perform forward FFT on in1
        std::copy(in1, in1 + d_fft_size, fft_fwd1.get_inbuf());
        fft_fwd1.execute();

        // Perform forward FFT on in2
        std::copy(in2, in2 + d_fft_size, fft_fwd2.get_inbuf());
        fft_fwd2.execute();

        // Calculate the conjugate of the second FFT and multiply it with the first FFT
        for (size_t i = 0; i < static_cast<size_t>(d_fft_size); ++i) {
            fft_fwd1.get_outbuf()[i] *= std::conj(fft_fwd2.get_outbuf()[i]);
        }

        // Perform inverse FFT to get the correlation
        std::copy(fft_fwd1.get_outbuf(), fft_fwd1.get_outbuf() + d_fft_size, fft_rev.get_inbuf());
        fft_rev.execute();

        // Find the index of the maximum value
        double max_norm = std::norm(fft_rev.get_outbuf()[0]);
        size_t index = 0;
        for (size_t i = 1; i < static_cast<size_t>(d_fft_size); ++i) {
            double curr_norm = std::norm(fft_rev.get_outbuf()[i]);
            if (curr_norm > max_norm) {
                max_norm = curr_norm;
                index = i;
            }
        }

        if(i%1000 == 0){
            std::cout << "Index of maximum correlation: " << index << std::endl;
        }
        i++;
        
        output_type* out1 = static_cast<output_type*>(output_items[0]);
        std::copy(in1, in1 + d_fft_size, out1);

        output_type* out2 = static_cast<output_type*>(output_items[1]);
        std::copy(in2, in2 + d_fft_size, out2);
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
