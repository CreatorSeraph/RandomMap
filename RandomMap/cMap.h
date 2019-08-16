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
	size_t minWidth;//BSP로 분할하는 공간
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
		if (_treeData.deep == _createData.deep)//이미 최대 깊이에 도달했을경우
		{
			return;
		}

		size_t& height = _treeData.height;
		size_t& width = _treeData.width;
		size_t& deep = _treeData.deep;

		bool isDivWidth = std::bernoulli_distribution(static_cast<float>(width) / (height + width))(_engine);//true = 가로로 분할, false = 세로로 분할
		size_t divVal = (isDivWidth ? width : height);
		size_t divMinVal = (isDivWidth ? _createData.minWidth : _createData.minHeight);
		size_t remainVal = (isDivWidth ? height : width);

		if (divVal <= divMinVal * 2)//분할할수 없는 경우
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

	//Noise를 사용한 맵생성
	//throw     : 맵 생성이 실패했을경우 logic_error, 맵변수나 랜덤시드가 잘못되었을경우 bad_argument
	//return    : this
	//_loopCount: 맵을 다듬어줄 횟수(높을수록 맵이 작게 생성되지만 원형에 가까운 모양을 가짐
	//_seed     : 맵을 생성하기 위해 필요한 시드값(없으면 랜덤한 값으로 생성됨)
	//_wallCount: 벽으로 바뀌기위해 필요한 인접한 벽의 개수
	//_cellRatio: 초기칸이 생길 확률(높을수록 벽이 적게 생성된다)
	cMap& CreateNoiseMap(size_t _loopCount, unsigned int _seed = std::random_device{}(), size_t _wallCount = 5, float _cellRatio = 0.45f);

	//BSP를 사용한 맵생성
	//throw : 맵 생성이 실패했을경우 logic_error, 맵변수나 랜덤시드가 잘못되었을경우 bad_argument
	//return: this
	//_deep : 맵의 깊이
	//_seed : 맵을 생성하기 위해 필요한 시드값(없으면 랜덤한 값으로 생성됨)
	cMap& CreateBSPMap(size_t _deep, const BSPTreeCreateData& _createData, unsigned int _seed = std::random_device{}());
	cMap& CreateBSPMap(size_t _deep, const cBSPTree& _createData, unsigned int _seed = std::random_device{}());
public:

};