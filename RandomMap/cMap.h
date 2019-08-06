#pragma once
#include <iostream>
#include <vector>
#include <random>

enum class TILE_TYPE : char
{
	GROUND, WALL, DOOR
};

class cBSPTree;
struct BSPTreeCreateData
{
	size_t width;
	size_t height;
	size_t minWidth;
	size_t minHeight;
};

class cBSPTree
{
public:
	cBSPTree* m_c1;
	cBSPTree* m_c2;
public:
	cBSPTree()
		:m_c1(nullptr), m_c2(nullptr)
	{
	}
	template <class _Engine>
	cBSPTree(_Engine _seed, float _childRatio)
	{
	}
};

class cMap
{
protected:
	friend std::ostream& operator<<(std::ostream& _out, const cMap& _map);
protected:
	using mapArrType = std::vector<TILE_TYPE>;
protected:
	mapArrType	m_map;
	size_t	m_width;
	size_t	m_height;
public:
	cMap(size_t _width, size_t _height);

	//Noise�� ����� �ʻ���
	//throw     : �� ������ ����������� logic_error, �ʺ����� �����õ尡 �߸��Ǿ������ bad_argument
	//return    : this
	//_loopCount: ���� �ٵ���� Ƚ��(�������� ���� �۰� ���������� ������ ����� ����� ����
	//_seed     : ���� �����ϱ� ���� �ʿ��� �õ尪(������ ������ ������ ������)
	//_wallCount: ������ �ٲ������ �ʿ��� ������ ���� ����
	//_cellRatio: �ʱ�ĭ�� ���� Ȯ��(�������� ���� ���� �����ȴ�)
	cMap& CreateNoiseMap(size_t _loopCount, unsigned int _seed = std::random_device{}(), size_t _wallCount = 5, float _cellRatio = 0.45f);

	//BSP�� ����� �ʻ���
	//throw : �� ������ ����������� logic_error, �ʺ����� �����õ尡 �߸��Ǿ������ bad_argument
	//return: this
	//_deep : ���� ����
	//_seed : ���� �����ϱ� ���� �ʿ��� �õ尪(������ ������ ������ ������)
	cMap& CreateBSPMap(size_t _deep, unsigned int _seed = std::random_device{}());
public:

};