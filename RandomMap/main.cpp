#include "cMap.h"

#include <list>
#include <vector>
#include <map>

using namespace std;

int main()
{
	cMap newMap(100, 100);

	//3568186414를 시드값으로 하면 길을 꺾어야 하는 경우가 나온다.
	cout << newMap.CreateBSPMap(BSPTreeCreateData(15, 15), BSPCellCreateData(6, 6)) << endl;

	//cout << newMap.CreateNoiseMap(3, std::random_device{}(), 5) << endl;//섬맵을 만들어본다
	//cout << newMap.CreateNoiseMap(3, std::random_device{}(), 6, 0.4f) << endl;//동굴맵을 만들어본다

	//cout << newMap;
	//std::normal_distribution d(0.0, 1.0);
	//bernoulli_distribution
}