cmake -S ./modules/eigen3 -B ./dependencies/eigen3 -DCMAKE_INSTALL_PREFIX="./install"
cmake --build ./dependencies/eigen3 --config Release --target install