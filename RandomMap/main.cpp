#include "cMap.h"

using namespace std;

int main()
{
	cMap newMap(100, 100);

	cout << newMap.CreateNoiseMap(3, std::random_device{}(), 5) << endl;//������ ������
	cout << newMap.CreateNoiseMap(3, std::random_device{}(), 6, 0.4f) << endl;//�������� ������

	//cout << newMap;
}