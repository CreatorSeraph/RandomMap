#include "cMap.h"

#include <list>
#include <vector>
#include <map>

using namespace std;

int main()
{
	cMap newMap(100, 100);

	newMap.CreateBSPMap(BSPTreeCreateData(10, 10, 4), BSPCellCreateData(6, 6));

	//cout << newMap.CreateNoiseMap(3, std::random_device{}(), 5) << endl;//������ ������
	//cout << newMap.CreateNoiseMap(3, std::random_device{}(), 6, 0.4f) << endl;//�������� ������

	//cout << newMap;
	//std::normal_distribution d(0.0, 1.0);
	//bernoulli_distribution
}