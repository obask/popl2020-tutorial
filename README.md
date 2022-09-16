# popl2020-tutorial
Tutorial for Building Program Reasoning Tools using LLVM and Z3

Running from command line:
```sh
mkdir build && cd build/
cmake ../CMakeLists.txt
cmake --build .

pushd ex1 && ctest
popd

```