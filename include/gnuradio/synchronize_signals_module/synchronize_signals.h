#ifndef INCLUDED_SYNCHRONIZE_SIGNALS_MODULE_SYNCHRONIZE_SIGNALS_H
#define INCLUDED_SYNCHRONIZE_SIGNALS_MODULE_SYNCHRONIZE_SIGNALS_H

#include <gnuradio/sync_block.h>
#include <gnuradio/synchronize_signals_module/api.h>

namespace gr {
namespace synchronize_signals_module {

/*!
 * \brief <+description of block+>
 * \ingroup synchronize_signals_module
 *
 */
class SYNCHRONIZE_SIGNALS_MODULE_API synchronize_signals : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<synchronize_signals> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of
     * synchronize_signals_module::synchronize_signals.
     *
     * To avoid accidental use of raw pointers,
     * synchronize_signals_module::synchronize_signals's constructor is in a private
     * implementation class. synchronize_signals_module::synchronize_signals::make is the
     * public interface for creating new instances.
     * \param synchronize The new synchronize state
     * \param fft_size Size of FFT
     */
    static sptr make(int fft_size, bool synchronize);
    /*!
     * \brief Set the synchronize state
     *
     * This method allows you to dynamically change the synchronize state of the block.
     *
     * \param synchronize The new synchronize state
     */
    virtual void set_synchronization(bool synchronize) = 0;
    virtual bool synchronize() const = 0;
};

} // namespace synchronize_signals_module
} // namespace gr

#endif /* INCLUDED_SYNCHRONIZE_SIGNALS_MODULE_SYNCHRONIZE_SIGNALS_H */
