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
	cMap& CreateBSPMap(size_t _deep, unsigned int _seed = std::random_device{}());
public:

};