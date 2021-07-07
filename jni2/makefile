
CXXFLAGS= -I.
CXXFLAGS += -I/home/d5000/jdk1.6/include
CXXFLAGS += -I/home/d5000/jdk1.6/include/linux
CXXFLAGS += -I/home/d5000/neimeng/sduept_promote/src/include/d5k_inc
LDFLAGS += -L/home/d5000/neimeng/lib

libmyjni.so:ComJni.cpp
	g++ -shared -fPIC ${CXXFLAGS} ${LDFLAGS} -lservices -o libmyjni.so ../ComJni.cpp
