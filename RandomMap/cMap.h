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

	//Noise를 사용한 맵생성
	//throw     : 맵 생성이 실패했을경우 logic_error, 맵변수나 랜덤시드가 잘못되었을경우 bad_argument
	//return    : this
	//_loopCount: 맵을 다듬어줄 횟수(높을수록 맵이 작게 생성되지만 원형에 가까운 모양을 가짐
	//_seed     : 맵을 생성하기 위해 필요한 시드값(없으면 랜덤한 값으로 생성됨)
	//_wallCount: 벽으로 바뀌기위해 필요한 인접한 벽의 개수
	//_cellRatio: 초기칸이 생길 확률(높을수록 벽이 적게 생성된다)
	cMap& CreateNoiseMap(size_t _loopCount, unsigned int _seed = std::random_device{}(), size_t _wallCount = 5, float _cellRatio = 0.45f);

	//BSP를 사용한 맵생성
	//throw	    : 맵 생성이 실패했을경우 logic_error, 맵변수나 랜덤시드가 잘못되었을경우 bad_argument
	//return    : this
	//_treeData : BSPTree생성에 필요한 인자
	//_cellData : BSPTree로 방을 만들때 필요한 인자
	//_seed     : 맵을 생성하기 위해 필요한 시드값(없으면 랜덤한 값으로 생성됨)
	cMap& CreateBSPMap(const BSPTreeCreateData& _treeData, const BSPCellCreateData& _cellData, unsigned int _seed = std::random_device{}());
public:
	inline size_t GetWidth() { return m_width; }
	inline size_t GetHeight() { return m_height; }
	TILE_TYPE& operator[](int idx) { return m_map[idx]; }
};