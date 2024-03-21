# gr-synchronize_signals_module
This GNU Radio module is created to synchronize multiple signals in samples and phase.

Signal from input port 'in0' is only copied to port 'out0'.
Every signals from other input ports are synchronized to first ('in0') signal and then copied to port corresponding port. Input port with index 'n' is copied (after changes) to output port 'n'.

Tested on GNU Radio Companion 3.10.7.0 (Python 3.10.12).

To build and test:
```
mkdir build 
cd build 
cmake .. 
make -j4 
sudo make install 
sudo ldconfig
gnuradio-companion ../examples/testSynchronizeSignals.grc
```


To rebuild (in build folder) and test:
```
sudo make uninstall
cd ..
sudo rm -r build
mkdir build 
cd build 
cmake .. 
make -j4 
sudo make install 
sudo ldconfig
gnuradio-companion ../examples/testSynchronizeSignals.grc
```
