#include "common.h" 

extern "C" {
	__declspec(dllexport) int Add(int x, int y) {
		return x + y;
	}
}