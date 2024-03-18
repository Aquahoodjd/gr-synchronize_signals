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

namespace gr {
namespace synchronize_signals_module {

using input_type = gr_complex;
using output_type = gr_complex;

const double PI = 3.14159265358979323846;

void fft(std::vector<std::complex<double>>& x) {
    const size_t N = x.size();
    if (N <= 1) return;

    // divide
    std::vector<std::complex<double>> even(N / 2);
    std::vector<std::complex<double>> odd(N / 2);
    for (size_t i = 0; i < N / 2; ++i) {
        even[i] = x[i * 2];
        odd[i] = x[i * 2 + 1];
    }

    // conquer
    fft(even);
    fft(odd);

    // combine
    for (size_t i = 0; i < N / 2; ++i) {
        std::complex<double> t = std::polar(1.0, -2 * PI * i / N) * odd[i];
        x[i] = even[i] + t;
        x[i + N / 2] = even[i] - t;
    }
}

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

        // Perform FFT on both inputs
        std::vector<std::complex<double>> fft1(in1, in1 + noutput_items);
        std::vector<std::complex<double>> fft2(in2, in2 + noutput_items);
        fft(fft1);
        fft(fft2);

        // Calculate the conjugate of the second FFT and multiply it with the first FFT
        for (size_t i = 0; i < static_cast<size_t>(noutput_items); ++i) {
            fft1[i] *= std::conj(fft2[i]);
        }

        // Perform inverse FFT to get the correlation
        fft(fft1);

        // Find the index of the maximum value
        double max_norm = std::norm(fft1[0]);
        size_t index = 0;
        for (size_t i = 1; i < fft1.size(); ++i) {
            double curr_norm = std::norm(fft1[i]);
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
