// synchronize_signals_impl.h
#ifndef INCLUDED_SYNCHRONIZE_SIGNALS_MODULE_SYNCHRONIZE_SIGNALS_IMPL_H
#define INCLUDED_SYNCHRONIZE_SIGNALS_MODULE_SYNCHRONIZE_SIGNALS_IMPL_H

#include <gnuradio/synchronize_signals_module/synchronize_signals.h>

namespace gr {
namespace synchronize_signals_module {

class synchronize_signals_impl : public synchronize_signals
{
private:
    bool d_synchronize;
    bool d_synchronize_state;
    int z;
    int d_fft_size;
    int index;
    double phase_difference;
    int d_num_ports;

public:
    synchronize_signals_impl(int fft_size, bool synchronize, int num_ports);
    ~synchronize_signals_impl();

    void set_synchronization(bool synchronize) override { d_synchronize = synchronize; }
    bool synchronize() const override { return d_synchronize; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace synchronize_signals_module
} // namespace gr

#endif /* INCLUDED_SYNCHRONIZE_SIGNALS_MODULE_SYNCHRONIZE_SIGNALS_IMPL_H */
