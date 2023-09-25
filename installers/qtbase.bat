cmake -S ./modules/qtbase -B ./dependencies/qtbase -G"Ninja Multi-Config" -DCMAKE_INSTALL_PREFIX="./install" -DCMAKE_C_COMPILER=cl.exe -DCMAKE_CXX_COMPILER=cl.exe -DCMAKE_BUILD_TYPE="Release"
cmake --build ./dependencies/qtbase
cd ./dependencies/qtbase
ninja install
cd ../../