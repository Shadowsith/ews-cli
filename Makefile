linux:
	g++ -std=c++17 -I/usr/include  -L/usr/include main.cpp -o build/ews-cli -lcurl

test_linux:
	g++ -std=c++17 -I/usr/include  -L/usr/include tests/test.cpp -o build/tests -lcurl
	./build/tests

windows:
	x86_64-w64-mingw32-g++ -I/usr/include test.cpp -o test -lcurl

clean:
	rm -f build/*
