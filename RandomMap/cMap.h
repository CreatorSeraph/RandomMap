#pragma once
#include <iostream>
#include <vector>
#include <random>
#include <memory>

enum class TILE_TYPE : char
{
	GROUND, WALL, DOOR
};

struct BSPTreeCreateData
{
	size_t minWidth;//BSP�� �����ϴ� ����
	size_t minHeight;
	size_t deep;

	BSPTreeCreateData(size_t _minWidth, size_t _minHeight, size_t _deep = 100)
		: minWidth(_minWidth), minHeight(_minHeight), deep(_deep)
	{
	}
};

struct BSPCellCreateData
{
	size_t minWidth;
	size_t minHeight;
	size_t remainLeft;
	size_t remainTop;
	size_t remainRight;
	size_t remainBottom;

	BSPCellCreateData(size_t _minWidth, size_t _minHeight)
		: minWidth(_minWidth), minHeight(_minHeight)
		, remainLeft(2), remainTop(2), remainRight(2), remainBottom(2)
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
	//throw	    : �� ������ ����������� logic_error, �ʺ����� �����õ尡 �߸��Ǿ������ bad_argument
	//return    : this
	//_treeData : BSPTree������ �ʿ��� ����
	//_cellData : BSPTree�� ���� ���鶧 �ʿ��� ����
	//_seed     : ���� �����ϱ� ���� �ʿ��� �õ尪(������ ������ ������ ������)
	cMap& CreateBSPMap(const BSPTreeCreateData& _treeData, const BSPCellCreateData& _cellData, unsigned int _seed = std::random_device{}());
public:
	inline size_t GetWidth() { return m_width; }
	inline size_t GetHeight() { return m_height; }
	TILE_TYPE& operator[](int idx) { return m_map[idx]; }
};