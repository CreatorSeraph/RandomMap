#include "cMap.h"

using namespace std;

int main()
{
	cMap newMap(100, 100);

	cout << newMap.CreateNoiseMap(3, std::random_device{}(), 5) << endl;//¼¶¸ÊÀ» ¸¸µé¾îº»´Ù
	cout << newMap.CreateNoiseMap(3, std::random_device{}(), 6, 0.4f) << endl;//µ¿±¼¸ÊÀ» ¸¸µé¾îº»´Ù

	//cout << newMap;
}