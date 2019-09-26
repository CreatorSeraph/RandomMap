#include "cMap.h"

#include <list>
#include <vector>
#include <map>

using namespace std;

int main()
{
	cMap newMap(50, 50);

	cout << newMap.CreateBSPMap(BSPTreeCreateData(15, 15), BSPCellCreateData(6, 6)) << endl;

	//cout << newMap.CreateNoiseMap(3, std::random_device{}(), 5) << endl;//¼¶¸ÊÀ» ¸¸µé¾îº»´Ù
	//cout << newMap.CreateNoiseMap(3, std::random_device{}(), 6, 0.4f) << endl;//µ¿±¼¸ÊÀ» ¸¸µé¾îº»´Ù

	//cout << newMap;
	//std::normal_distribution d(0.0, 1.0);
	//bernoulli_distribution
}