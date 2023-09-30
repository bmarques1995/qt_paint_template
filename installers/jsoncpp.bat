cmake -S ./modules/jsoncpp -B ./dependencies/jsoncpp -DCMAKE_INSTALL_PREFIX="./install"
cmake --build ./dependencies/jsoncpp --config Debug --target install