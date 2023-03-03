release:
	aarch64-oe-linux-g++ --sysroot=$(TOOLCHAIN_ROOT)/aarch64-oe-linux -isystem $(BOOST_INCLUDE) -march=armv8-a -mcpu=cortex-a57 -mtune=cortex-a57 -pipe -fomit-frame-pointer -Wa,--noexecstack -Wall -Wextra -O1 -o test main.cpp -L$(BOOST_LIB) -lboost_system -lpthread
	rsync -avh --progress test car:/run/

debug:
	aarch64-oe-linux-g++ --sysroot=$(TOOLCHAIN_ROOT)/aarch64-oe-linux -isystem $(BOOST_INCLUDE) -march=armv8-a -mcpu=cortex-a57 -mtune=cortex-a57 -pipe -fomit-frame-pointer -Wa,--noexecstack -Wall -Wextra -Os -g -o test main.cpp -L$(BOOST_LIB) -lboost_system -lpthread
