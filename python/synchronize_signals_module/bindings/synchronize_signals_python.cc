#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/synchronize_signals_module/synchronize_signals.h>
#include <synchronize_signals_pydoc.h>

void bind_synchronize_signals(py::module& m)
{
    using synchronize_signals = gr::synchronize_signals_module::synchronize_signals;
    py::class_<synchronize_signals,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<synchronize_signals>>(
        m, "synchronize_signals", D(synchronize_signals))
        .def(py::init(&synchronize_signals::make),
             py::arg("fft_size"),
             py::arg("synchronize"),
             D(synchronize_signals, make))
        .def("set_synchronization",
             &synchronize_signals::set_synchronization,
             py::arg("synchronize"),
             D(synchronize_signals, set_synchronization))
        .def("synchronize",
             &synchronize_signals::synchronize,
             D(synchronize_signals, synchronize));
}
