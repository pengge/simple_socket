#!/usr/bin/env sh

#sudo echo  /usr/local/lib >>/etc/ld.so.conf
#sudo ldconfig

pwd
# build glog
cd ..
git clone -b v0.3.4 https://github.com/google/glog.git --single-branch --depth=1
cd glog
./configure
autoreconf -ivf
make
sudo make install
sudo cp /usr/local/lib/libglog.* /usr/lib/
cd ..
pwd

# build gtest
git clone -b release-1.10.0 https://github.com/google/googletest.git  --single-branch --depth=1
cd googletest
cmake CMakeLists.txt
make
sudo make install
cd ..
pwd

# add externals
git clone -b develop https://github.com/butterflyy/externals.git  --single-branch --depth=1
pwd

#build simple_server_socket
cd simple_socket
cd api/simple_server_socket
sudo ./make.sh

#build simple_server_socket
cd ../simple_client_socket
sudo ./make.sh

#build UnitTest
cd ../../test/UnitTest/UnitTest
make

#run UnitTest
./UnitTest
