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

synchronize_signals::sptr
synchronize_signals::make(int fft_size, bool synchronize, int num_ports)
{
    return gnuradio::make_block_sptr<synchronize_signals_impl>(
        fft_size, synchronize, num_ports);
}

synchronize_signals_impl::synchronize_signals_impl(int fft_size,
                                                   bool synchronize,
                                                   int num_ports)
    : gr::sync_block(
          "synchronize_signals",
          gr::io_signature::make(
              2 /* min inputs */, -1 /* max inputs */, fft_size * sizeof(gr_complex)),
          gr::io_signature::make(
              2 /* min outputs */, -1 /*max outputs */, fft_size * sizeof(gr_complex))),
      d_synchronize(synchronize),
      d_synchronize_state(false),
      z(0),
      d_fft_size(fft_size),
      d_num_ports(num_ports),
      index(num_ports, 0),
      phase_difference(num_ports, 0)
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

    const input_type* in0 = static_cast<const input_type*>(input_items[0]);
    std::vector<std::vector<input_type>> in_synchronized(
        d_num_ports, std::vector<input_type>(d_fft_size, 0));

    if (d_synchronize == true) {
        fft_complex_fwd fft_fwd0(d_fft_size);

        // Perform forward FFT on in0
        std::copy(in0, in0 + d_fft_size, fft_fwd0.get_inbuf());
        fft_fwd0.execute();

        for (int port = 1; port < d_num_ports; ++port) {
            const input_type* in = static_cast<const input_type*>(input_items[port]);
            fft_complex_fwd fft_fwd(d_fft_size);
            fft_complex_rev fft_rev(d_fft_size);
            std::vector<std::complex<float>> fft_conj_mult(d_fft_size);

            // Perform forward FFT on in
            std::copy(in, in + d_fft_size, fft_fwd.get_inbuf());
            fft_fwd.execute();

            // Calculate the conjugate of the second FFT and multiply it with the first
            // FFT
            for (size_t i = 0; i < static_cast<size_t>(d_fft_size); ++i) {
                fft_conj_mult[i] =
                    fft_fwd0.get_outbuf()[i] * std::conj(fft_fwd.get_outbuf()[i]);
            }

            // Perform inverse FFT to get the correlation
            std::copy(fft_conj_mult.begin(), fft_conj_mult.end(), fft_rev.get_inbuf());
            fft_rev.execute();
            // Find the index of the maximum value
            double max_norm = std::norm(fft_rev.get_outbuf()[0]);
            for (int i = 1; i < d_fft_size; ++i) { // Change size_t to int
                double curr_norm = std::norm(fft_rev.get_outbuf()[i]);
                if (curr_norm > max_norm) {
                    max_norm = curr_norm;
                    index[port] = i;
                }
            }

            // Unwrap the index[port] if it's greater than half of the FFT size
            if (index[port] > d_fft_size / 2) {
                index[port] -= d_fft_size;
            }

            // Calculate the phase difference at the point of maximum correlation
            int adjusted_index = index[port] < 0 ? index[port] + d_fft_size : index[port];
            phase_difference[port] = std::arg(fft_rev.get_outbuf()[adjusted_index]);

            // Copy the input signal into the new buffer with the appropriate delay
            for (int i = 0; i < d_fft_size; ++i) {
                int j = (i - index[port]) % d_fft_size;
                if (j < 0) {
                    j += d_fft_size;
                }
                in_synchronized[port][i] = in[j];
            }

            // Adjust the phase of in_synchronized based on the phase difference
            if (abs(phase_difference[port]) > 0.001) {
                for (int i = 0; i < d_fft_size; ++i) {
                    double magnitude = std::abs(in_synchronized[port][i]);
                    double phase =
                        std::arg(in_synchronized[port][i]) + phase_difference[port];
                    in_synchronized[port][i] = std::polar(magnitude, phase);
                }
            }

            output_type* out = static_cast<output_type*>(output_items[port]);
            std::copy(in_synchronized[port].begin(), in_synchronized[port].end(), out);
        }
    } else {
        for (int port = 1; port < d_num_ports; ++port) {
            const input_type* in = static_cast<const input_type*>(input_items[port]);
            if (index[port] != 0 || abs(phase_difference[port]) > 0.001) {
                // Copy the input signal into the new buffer with the appropriate delay
                for (int i = 0; i < d_fft_size; ++i) {
                    int j = (i - index[port]) % d_fft_size;
                    if (j < 0) {
                        j += d_fft_size;
                    }
                    in_synchronized[port][i] = in[j];
                }

                // Adjust the phase of in_synchronized[port] based on the phase difference
                if (abs(phase_difference[port]) > 0.001) {
                    for (int i = 0; i < d_fft_size; ++i) {
                        double magnitude = std::abs(in_synchronized[port][i]);
                        double phase =
                            std::arg(in_synchronized[port][i]) + phase_difference[port];
                        in_synchronized[port][i] = std::polar(magnitude, phase);
                    }
                }
                if (abs(phase_difference[port]) <= 0.001 && index[port] == 0) {
                    for (int i = 0; i < d_fft_size; ++i) {
                        in_synchronized[port][i] = in[i];
                    }
                }

                output_type* out = static_cast<output_type*>(output_items[port]);
                std::copy(
                    in_synchronized[port].begin(), in_synchronized[port].end(), out);
            } else {
                output_type* out = static_cast<output_type*>(output_items[port]);
                std::copy(in, in + d_fft_size, out);
            }
        }
    }
    output_type* out0 = static_cast<output_type*>(output_items[0]);
    std::copy(in0, in0 + d_fft_size, out0);

    return noutput_items;
}
} /* namespace synchronize_signals_module */
} /* namespace gr */
