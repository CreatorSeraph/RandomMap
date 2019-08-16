#include "cMap.h"

#include <list>
#include <vector>
#include <map>

using namespace std;

class Test
{
private:
	int val;
	string str;
public:
	Test(const string& _str, int _val)
		:str(_str), val(_val)
	{
	}
};

void TestFunc()
{
	//list<Test> l;
	//l.push_back(Test("hi", 1));
	//l.push_back("hi", 1);
	//l.emplace_back(Test("hi", 1));
	//l.emplace_back("hi", 1);
	//vector<Test> v;

	int j = 0;
}

int main()
{
	cMap newMap(100, 100);

	newMap.CreateBSPMap(3, BSPTreeCreateData(4, 4, 4));

	//cout << newMap.CreateNoiseMap(3, std::random_device{}(), 5) << endl;//¼¶¸ÊÀ» ¸¸µé¾îº»´Ù
	//cout << newMap.CreateNoiseMap(3, std::random_device{}(), 6, 0.4f) << endl;//µ¿±¼¸ÊÀ» ¸¸µé¾îº»´Ù

	//cout << newMap;
	//std::normal_distribution d(0.0, 1.0);
	//bernoulli_distribution
}