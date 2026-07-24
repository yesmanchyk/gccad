# Automatic Differentiation with GCC

Requires https://github.com/yesmanchyk/gcc-mirror/tree/body_of

```
git clone --branch body_of --depth 1 \
  https://github.com/yesmanchyk/gcc-mirror
docker build -t gccb .
docker run --rm -it -v `pwd`:/src gccb
cd gcc-mirror # or /src/gcc-mirror
contrib/download_prerequisites
cd ..
mkdir build
cd build
../gcc-mirror/configure --enable-languages=c,c++ \
  --disable-bootstrap --disable-multilib --disable-nls \
  --prefix=/src/local
make -j8
make install
cd ..
local/bin/g++ -std=c++26 -freflection main.cpp -o generator
./generator
```
The generator should produce `derivatives.h` which can be included into an application like `test_derivatives.cpp`

Run GCC tests for reflection
```
cd build/gcc
make check-g++ RUNTESTFLAGS="dg.exp=reflect/*.C"
```
