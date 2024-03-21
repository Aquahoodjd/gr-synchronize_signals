// synchronize_signals_impl.cc
#include "synchronize_signals_impl.h"
#include <gnuradio/fft/fft.h>
#include <gnuradio/io_signature.h>
#include <cmath>
#include <complex>
#include <vector>

namespace gr {
namespace synchronize_signals_module {

using fft_complex_fwd = gr::fft::fft<gr_complex, true>;
using fft_complex_rev = gr::fft::fft<gr_complex, false>;
using input_type = gr_complex;
using output_type = gr_complex;

const double PI = 3.14159265358979323846;

synchronize_signals::sptr synchronize_signals::make(int fft_size, bool synchronize)
{
    return gnuradio::make_block_sptr<synchronize_signals_impl>(fft_size, synchronize);
}

synchronize_signals_impl::synchronize_signals_impl(int fft_size, bool synchronize)
    : gr::sync_block(
          "synchronize_signals",
          gr::io_signature::make(
              2 /* min inputs */, 2 /* max inputs */, fft_size * sizeof(gr_complex)),
          gr::io_signature::make(
              2 /* min outputs */, 2 /*max outputs */, fft_size * sizeof(gr_complex))),
      d_synchronize(synchronize),
      d_synchronize_state(false),
      z(0),
      d_fft_size(fft_size),
      index(0),
      phase_difference(0)
{
}

synchronize_signals_impl::~synchronize_signals_impl() {}

int synchronize_signals_impl::work(int noutput_items,
                                   gr_vector_const_void_star& input_items,
                                   gr_vector_void_star& output_items)
{
    if (d_synchronize != d_synchronize_state) {
        d_synchronize_state = d_synchronize;
    }

    const input_type* in1 = static_cast<const input_type*>(input_items[0]);
    const input_type* in2 = static_cast<const input_type*>(input_items[1]);
    std::vector<input_type> in2_synchronized(d_fft_size, 0);

    if (d_synchronize == true) {
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
        std::copy(fft_fwd1.get_outbuf(),
                  fft_fwd1.get_outbuf() + d_fft_size,
                  fft_rev.get_inbuf());
        fft_rev.execute();

        // Find the index of the maximum value
        double max_norm = std::norm(fft_rev.get_outbuf()[0]);
        for (int i = 1; i < d_fft_size; ++i) { // Change size_t to int
            double curr_norm = std::norm(fft_rev.get_outbuf()[i]);
            if (curr_norm > max_norm) {
                max_norm = curr_norm;
                index = i;
            }
        }

        // Unwrap the index if it's greater than half of the FFT size
        if (index > d_fft_size / 2) {
            index -= d_fft_size;
        }

        // Calculate the phase difference at the point of maximum correlation
        int adjusted_index = index < 0 ? index + d_fft_size : index;
        phase_difference = std::arg(fft_rev.get_outbuf()[adjusted_index]);

        // Copy the input signal into the new buffer with the appropriate delay
        if (index != 0) {
            for (int i = 0; i < d_fft_size; ++i) {
                int j = (i - index) % d_fft_size;
                if (j < 0) {
                    j += d_fft_size;
                }
                in2_synchronized[i] = in2[j];
            }
        }

        // Adjust the phase of in2_synchronized based on the phase difference
        if (abs(phase_difference) > 0.001) {
            for (int i = 0; i < d_fft_size; ++i) {
                double magnitude = std::abs(in2_synchronized[i]);
                double phase = std::arg(in2_synchronized[i]) + phase_difference;
                in2_synchronized[i] = std::polar(magnitude, phase);
            }
        }

        output_type* out1 = static_cast<output_type*>(output_items[0]);
        output_type* out2 = static_cast<output_type*>(output_items[1]);
        std::copy(in1, in1 + d_fft_size, out1);
        std::copy(in2_synchronized.begin(), in2_synchronized.end(), out2);
    } else {
        if (index != 0 || abs(phase_difference) > 0.001) {
            // Copy the input signal into the new buffer with the appropriate delay
            if (index != 0) {
                for (int i = 0; i < d_fft_size; ++i) {
                    int j = (i - index) % d_fft_size;
                    if (j < 0) {
                        j += d_fft_size;
                    }
                    in2_synchronized[i] = in2[j];
                }
            }

            // Adjust the phase of in2_synchronized based on the phase difference
            if (abs(phase_difference) > 0.001) {
                for (int i = 0; i < d_fft_size; ++i) {
                    double magnitude = std::abs(in2_synchronized[i]);
                    double phase = std::arg(in2_synchronized[i]) + phase_difference;
                    in2_synchronized[i] = std::polar(magnitude, phase);
                }
            }

            output_type* out1 = static_cast<output_type*>(output_items[0]);
            output_type* out2 = static_cast<output_type*>(output_items[1]);
            std::copy(in1, in1 + d_fft_size, out1);
            std::copy(in2_synchronized.begin(), in2_synchronized.end(), out2);
        } else {
            output_type* out1 = static_cast<output_type*>(output_items[0]);
            output_type* out2 = static_cast<output_type*>(output_items[1]);
            std::copy(in1, in1 + d_fft_size, out1);
            std::copy(in2, in2 + d_fft_size, out2);
        }
    }

    return noutput_items;
}
} /* namespace synchronize_signals_module */
} /* namespace gr */
