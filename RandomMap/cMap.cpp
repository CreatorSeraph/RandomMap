#include "cMap.h"
using namespace std;

cMap::cMap(size_t _width, size_t _height)
	:m_map(_width * _height, TILE_TYPE::GROUND), m_width(_width), m_height(_height)
{
}

ostream& operator<<(ostream& _out, const cMap& _map)
{
	const auto& mapWidth = _map.m_width;
	const auto& mapHeight = _map.m_height;
	auto& mapTile = _map.m_map;
	size_t loopCount = 0;

	for (auto iter : mapTile)
	{
		_out << (iter == TILE_TYPE::GROUND ? "□" : "■");
		if (++loopCount == mapWidth)
		{
			_out << endl;
			loopCount = 0;
		}
	}

	return _out;
}

//Noise를 사용한 맵생성
//throw     : 맵 생성이 실패했을경우 logic_error, 맵변수나 랜덤시드가 잘못되었을경우 bad_argument
//return    : this
//_loopCount: 맵을 다듬어줄 횟수(높을수록 맵이 작게 생성되지만 원형에 가까운 모양을 가짐
//_seed     : 맵을 생성하기 위해 필요한 시드값(없으면 랜덤한 값으로 생성됨)
//_wallCount: 벽으로 바뀌기위해 필요한 인접한 벽의 개수
//_cellRatio: 초기칸이 생길 확률(높을수록 벽이 적게 생성된다)
cMap& cMap::CreateNoiseMap(size_t _loopCount, unsigned int _seed, size_t _wallCount, float _cellRatio)
{
	constexpr bool printLog = false;

	//시드값에서 랜덤엔진을 생성
	mt19937_64 randEngine(_seed);

	auto& mapTile = m_map;
	const auto& mapWidth = m_width;
	const auto& mapHeight = m_height;
	auto GetIdx = [mapWidth](int y, int x) { return x + y * mapWidth; };//인덱스를 구해주는 함수
	mapArrType prevMapTile(mapWidth * mapHeight);//이전맵을 토대로 맵을 다듬을것이기때문에 현재맵과 똑같은 사이즈의 맵을 하나 만들어준다.

	auto FindAdjacentGroundCount = [&prevMapTile, GetIdx, mapWidth, mapHeight](int y, int x) {//인접한 벽의 개수를 구하는 람다함수
		size_t result = 0;
		int yTarget = min(y + 1, static_cast<int>(mapHeight) - 1);
		int xTarget = min(x + 1, static_cast<int>(mapWidth) - 1);//매 반복을 돌때마다 계산하는것보다 미리 계산된값을 쓰는것이 좋다.
		for (int i = max(y - 1, 0); i <= yTarget; ++i)
		{
			for (int j = max(x - 1, 0); j <= xTarget; ++j)
			{
				result += (prevMapTile[GetIdx(i, j)] == TILE_TYPE::GROUND);//bool형식은 정수형으로 변환될때 false는 0, true는 1로 바뀐다.
			}
		}

		return result;
	};

	uniform_real_distribution<float> randTileSet(0, 1);
	for (auto& iter : mapTile)
	{
		auto checkVal = randTileSet(randEngine);
		iter = ((checkVal < _cellRatio) ?
			TILE_TYPE::GROUND :
			TILE_TYPE::WALL);
	}//랜덤하게 초기값을 생성해준다.

	if constexpr(printLog)
		cout << *this << endl;

	while (_loopCount-- > 0)
	{
		prevMapTile = mapTile;
		for (int i = 0; i < mapHeight; ++i)
		{
			for (int j = 0; j < mapWidth; ++j)
			{
				mapTile[GetIdx(i, j)] = (FindAdjacentGroundCount(i, j) > 9 - _wallCount) ? TILE_TYPE::GROUND : TILE_TYPE::WALL;
			}
		}
		if constexpr (printLog)
			cout << *this << endl;
	}
	return *this;
}

//BSP를 사용한 맵생성
//throw : 맵 생성이 실패했을경우 logic_error, 맵변수나 랜덤시드가 잘못되었을경우 bad_argument
//return: this
//_deep : 맵의 깊이
//_seed : 맵을 생성하기 위해 필요한 시드값(없으면 랜덤한 값으로 생성됨)
cMap& cMap::CreateBSPMap(size_t _deep, unsigned int _seed)
{
	std::mt19937_64 randEngine(_seed);
	return *this;
}