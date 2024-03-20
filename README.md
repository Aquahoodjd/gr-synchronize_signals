# gr-synchronize_signals_module
This module is created to synchronize two signals in samples and phase.
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
