This fork was moved to CMake for easier build and dependency resolution.


Building and Installation:

For Linux

These steps only applies to Debian or other Debian-based distribution.
(Tested on my Debian Jessie installation, had also succeeded building in Ubuntu 16.04 LTS Xenial) 


1. Install required packages

		$ sudo apt-get install libfox-1.6-dev libglib2.0-dev libssl-dev libsqlite3-dev -y

2. Create a directory for building.

		$ mkdir server/build; cd server/build
		$ cmake ..
		$ make -j4
		$ sudo make install

	This will install the following files:

		/usr/local/lib/libccls.so.0.8.2
		/usr/local/lib/libccls.so.0
		/usr/local/lib/libccls.so
		/usr/local/include/ccls.h
		/usr/local/bin/mkahawa-srv

3. Add path to the shared object (i.e libccls.so):

		$ sudo sh -c "echo /usr/local/lib/ >> /etc/ld.so.conf.d/libccl.conf"
		$ sudo ldconfig

4. Run mkahawa server

		$ mkahawa-srv

The installation for mkahawa client is pretty much the same.
Just do the same for the cafeman/client directory.
