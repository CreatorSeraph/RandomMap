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

	BSPTreeCreateData(size_t _minWidth, size_t _minHeight, size_t _deep)
		:minWidth(_minWidth), minHeight(_minHeight), deep(_deep)
	{
	}
};

struct BSPCellCreateData
{
	size_t minWidth;
	size_t minHeight;

	BSPCellCreateData(size_t _minWidth, size_t _minHeight)
		:minWidth(_minWidth), minHeight(_minHeight)
	{
	}
};

struct BSPTreeData
{
	size_t width;
	size_t height;
	size_t deep;

	BSPTreeData() = default;
	BSPTreeData(size_t _width, size_t _height, size_t _deep = 0)
		:width(_width), height(_height), deep(_deep)
	{
	}
};

class cBSPTree
{
public:
	cBSPTree* m_c1;
	cBSPTree* m_c2;
	BSPTreeData m_data;
public:
	template <class _Engine>
	cBSPTree(_Engine& _engine, const BSPTreeCreateData& _createData, BSPTreeData&& _treeData)
		:m_c1(nullptr), m_c2(nullptr), m_data(_treeData)
	{
		if (_treeData.deep == _createData.deep)//�̹� �ִ� ���̿� �����������
		{
			return;
		}

		size_t& height = _treeData.height;
		size_t& width = _treeData.width;
		size_t& deep = _treeData.deep;

		bool isDivWidth = std::bernoulli_distribution(static_cast<float>(width) / (height + width))(_engine);//true = ���η� ����, false = ���η� ����
		size_t divVal = (isDivWidth ? width : height);
		size_t divMinVal = (isDivWidth ? _createData.minWidth : _createData.minHeight);
		size_t remainVal = (isDivWidth ? height : width);

		if (divVal <= divMinVal * 2)//�����Ҽ� ���� ���
		{
			return;
		}

		size_t divResult = uniform_int_distribution<size_t>(divMinVal, divVal)(_engine);
		//size_t divResult = divMinVal + std::binomial_distribution(divVal - divMinVal, 0.5)(_engine);
		size_t divRemain = divVal - divResult;

		auto makeTreeData = [isDivWidth, remainVal, deep](size_t val) {
			return BSPTreeData(isDivWidth ? val : remainVal, isDivWidth ? remainVal : val, deep + 1);
		};

		m_c1 = new cBSPTree(_engine, _createData, makeTreeData(divResult));
		m_c2 = new cBSPTree(_engine, _createData, makeTreeData(divRemain));
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
	cMap& CreateBSPMap(size_t _deep, const BSPTreeCreateData& _createData, unsigned int _seed = std::random_device{}());
	cMap& CreateBSPMap(size_t _deep, const cBSPTree& _createData, unsigned int _seed = std::random_device{}());
public:

};