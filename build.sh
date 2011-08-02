
gcc -Wl,-rpath,$HOME/local-gcc-trunk/lib64 \
    -Wl,-rpath,$HOME/local-gcc-trunk/lib \
    -Wl,-rpath,$HOME/local-gcc-trunk/boost-trunk/lib \
    -I $HOME/local-gcc-trunk/include \
    -L $HOME/local-gcc-trunk/lib64 \
    -L $HOME/local-gcc-trunk/lib \
    -I $HOME/local-gcc-trunk/boost-trunk/include \
    -L $HOME/local-gcc-trunk/boost-trunk/lib \
    -lstdc++ \
    -I /usr/include/X11 \
    -L /usr/lib/X11 \
    -lX11 \
    -Wall -fpic -O3 -s \
    -std=c++0x \
    -o dwmstatus \
    dwmstatus.cpp


