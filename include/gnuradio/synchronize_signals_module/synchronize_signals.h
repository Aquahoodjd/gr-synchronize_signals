// synchronize_signals.h
#ifndef INCLUDED_SYNCHRONIZE_SIGNALS_MODULE_SYNCHRONIZE_SIGNALS_H
#define INCLUDED_SYNCHRONIZE_SIGNALS_MODULE_SYNCHRONIZE_SIGNALS_H

#include <gnuradio/sync_block.h>
#include <gnuradio/synchronize_signals_module/api.h>

namespace gr {
namespace synchronize_signals_module {

/*!
 * \brief This class represents a block that synchronizes signals in a GNU Radio flowgraph.
 * \ingroup synchronize_signals_module
 *
 */
class SYNCHRONIZE_SIGNALS_MODULE_API synchronize_signals : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<synchronize_signals> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of synchronize_signals_module::synchronize_signals.
     *
     * \param fft_size The size of the Fast Fourier Transform (FFT) to be used.
     * \param synchronize The initial synchronization state. If true, the block will start in synchronization mode.
     * \param num_ports The number of input and output ports.
     */
    static sptr make(int fft_size, bool synchronize, int num_ports);
    
    /*!
     * \brief Set the synchronization state.
     *
     * This method allows you to dynamically change the synchronization state of the block.
     *
     * \param synchronize The new synchronization state. If true, the block will switch to synchronization mode.
     */
    virtual void set_synchronization(bool synchronize) = 0;
    virtual bool synchronize() const = 0;
};

} // namespace synchronize_signals_module
} // namespace gr

#endif /* INCLUDED_SYNCHRONIZE_SIGNALS_MODULE_SYNCHRONIZE_SIGNALS_H */
