#include "cMap.h"
#include <variant>
#include <utility>
#include <list>
using namespace std;

cMap::cMap(size_t _width, size_t _height)
	: m_map(_width * _height, TILE_TYPE::GROUND), m_width(_width), m_height(_height)
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
		for (size_t i = 0; i < mapHeight; ++i)
		{
			for (size_t j = 0; j < mapWidth; ++j)
			{
				mapTile[GetIdx(i, j)] = (FindAdjacentGroundCount(i, j) > 9 - _wallCount) ? TILE_TYPE::GROUND : TILE_TYPE::WALL;
			}
		}
		if constexpr (printLog)
			cout << *this << endl;
	}
	return *this;
}

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>;//오버로드 패턴 사용

struct BSPCellData
{
	size_t left;
	size_t right;
	size_t top;
	size_t bottom;

	BSPCellData(const BSPCellData& _other) = default;
	BSPCellData(size_t _width, size_t _height)
		: left(0), right(_width), top(0), bottom(_height)
	{
	}

	size_t width() const { return right - left; }
	size_t height() const { return bottom - top; }

	std::pair<size_t, size_t> GetDistance(const BSPCellData& _other)
	{
		return make_pair(GetXDistance(_other), GetYDistance(_other));
	}

	size_t GetXDistance(const BSPCellData& _other)
	{
		//값들을 2배로 사용하는 이유는 float로 잘릴수 있기 때문. 타일맵이기때문에 굳이 float자료형을 사용하지 않는다.
		size_t xDistance2 = labs((left + right) - (_other.left + _other.right));//두 사각형의 x축 거리(의 2배)
		size_t widthSum = width() + _other.width();//두 사각형의 너비의 합(의 2배)
		//xDistance <= widthSum / 2
		//xDistance * 2 <= widthSum
		if (xDistance2 < widthSum)//두 사각형의 거리가, 너비의 합의 절반보다 클경우 (원충돌검사하듯이 x축 검사)
			xDistance2 = widthSum;//거리를 구하는데 음수의 거리가 나올순 없다.

		//(xDistnace - widthSum / 2)
		//(xDistnace2 - widthSum) / 2
		return (xDistance2 - widthSum) / 2;
	}

	size_t GetYDistance(const BSPCellData& _other)
	{
		size_t yDistance2 = labs((top + bottom) - (_other.top + _other.bottom));
		size_t heightSum = height() + _other.height();
		if (yDistance2 <= heightSum)//반복
			yDistance2 = heightSum;

		return (yDistance2 - heightSum) / 2;
	}
};

class BSPTree
{
private:
	struct BSPChild
	{
		BSPTree* c1;
		BSPTree* c2;

		BSPChild(BSPTree* _c1, BSPTree* _c2)
			: c1(_c1), c2(_c2)
		{
		}
	};
	variant<BSPCellData, BSPChild> m_data;
	size_t m_divLine;
	bool m_isDivWidth;
public:
	template <class _Engine>
	BSPTree(_Engine& _engine, const BSPTreeCreateData& _createData, const BSPCellData& _mapData, size_t _deep = 0)
		: m_data(_mapData)//셀을 만들기 위한 정보를 넘겨준다.
	{
		cout << _mapData.left << "\t" << _mapData.right << "\t" << _mapData.top << "\t" << _mapData.bottom;
		size_t width = _mapData.width();
		size_t height = _mapData.height();

		size_t xMinVal = _mapData.left + _createData.minWidth;
		size_t xMaxVal = _mapData.right - _createData.minWidth;
		size_t yMinVal = _mapData.top + _createData.minHeight;
		size_t yMaxVal = _mapData.bottom - _createData.minWidth;

		int xDivVal = xMaxVal - xMinVal;
		int yDivVal = yMaxVal - yMinVal;

		if (xDivVal < 0)
			xDivVal = -1;
		if (yDivVal < 0)
			yDivVal = -1;

		//이미 최대 깊이에 도달했거나 분할 가능한 너비가 없는 경우
		if (_deep == _createData.deep || (xDivVal == -1 && yDivVal == -1))
		{
			m_data = std::move(_mapData);//셀을 만들기 위한 정보를 넘겨준다.
			cout << "마지막 노드" << endl;
			return;
		}

		m_isDivWidth = std::bernoulli_distribution(static_cast<float>(xDivVal + 1) / (xDivVal + yDivVal + 2))(_engine);//true = 가로로 분할, false = 세로로 분할
		size_t divMaxVal = (m_isDivWidth ? xMaxVal : yMaxVal);
		size_t divMinVal = (m_isDivWidth ? xMinVal : yMinVal);

		BSPCellData t1(_mapData);
		BSPCellData t2(_mapData);

		(m_isDivWidth ? t1.right : t1.bottom) = (m_isDivWidth ? t2.left : t2.top) = m_divLine
			= uniform_int_distribution<size_t>(divMinVal, divMaxVal)(_engine);

		cout << endl;

		m_data = BSPChild(
			new BSPTree(_engine, _createData, std::move(t1), _deep + 1),
			new BSPTree(_engine, _createData, std::move(t2), _deep + 1)
		);
	}

	template <class _Engine>
	std::list<BSPCellData> CreateMap(_Engine& _engine, cMap& _map, const BSPCellCreateData& _cellData)
	{
		if (std::holds_alternative<BSPCellData>(m_data))
		{
			//노드의 마지막 부분이다. 맵생성이 필요함
			//BSPCellData result;

			auto& data = std::get<BSPCellData>(m_data);
			data.left += _cellData.remainLeft;
			data.right -= _cellData.remainRight;
			data.top += _cellData.remainTop;
			data.bottom -= _cellData.remainBottom;

			auto subWidth = uniform_int_distribution<size_t>(0, data.width() - _cellData.minWidth)(_engine);
			auto subHeight = uniform_int_distribution<size_t>(0, data.height() - _cellData.minHeight)(_engine);
			auto offsetLeft = uniform_int_distribution<size_t>(0, subWidth)(_engine);
			auto offsetTop = uniform_int_distribution<size_t>(0, subHeight)(_engine);

			data.left += offsetLeft;
			data.top += offsetTop;
			data.right -= subWidth - offsetLeft;
			data.bottom -= subHeight - offsetTop;

			auto mapWidth = _map.GetWidth();
			auto GetIdx = [mapWidth](int y, int x) { return x + y * mapWidth; };//인덱스를 구해주는 함수

			for (size_t i = data.left; i <= data.right; ++i)
			{
				for (size_t j = data.top; j <= data.bottom; ++j)
				{
					_map[GetIdx(j, i)] = TILE_TYPE::GROUND;
				}
			}

			//만든 방의 크기를 반환하여 길을 연결시킬때 도움이 되도록 한다.
			return { data };
		}
		//자식들에게 맵을 생성하게 시킨다
		std::list<BSPCellData> result;
		auto& child = std::get<BSPChild>(m_data);
		auto cellList1 = child.c1->CreateMap(_engine, _map, _cellData);
		auto cellList2 = child.c2->CreateMap(_engine, _map, _cellData);

		auto resultIter1 = cellList1.begin();
		auto resultIter2 = cellList2.begin();
		size_t minXDistance = _map.GetWidth();
		size_t minYDistance = _map.GetHeight();
		for (auto iter1 = resultIter1; iter1 != cellList1.end(); ++iter1)
		{
			for (auto iter2 = resultIter2; iter2 != cellList2.end(); ++iter2)
			{
				auto [xDistTemp, yDistTemp] = (*iter1).GetDistance(*iter2);
				if (xDistTemp + yDistTemp < minXDistance + minYDistance)
				{
					minXDistance = xDistTemp;
					minYDistance = yDistTemp;
					resultIter1 = iter1;
					resultIter2 = iter2;
				}
			}
		}

		auto& cell1 = *resultIter1;
		auto& cell2 = *resultIter2;
		//가장 거리가 짧은 두개의 cell을 구한다

		int startX, startY;
		int endX, endY;
		if (cell1.left > cell2.right)//cell1이 cell2의 오른쪽에 있다.
		{
			startX = cell2.right;
			endX = cell1.left;
		}
		else if (cell2.left > cell1.right)//cell2가 cell1의 오른쪽에 있다.
		{
			startX = cell1.right;
			endX = cell2.left;
		}
		else//두 사각형의 X축에서 겹치는 부분이 있다.
		{
			//겹치는 부분(left의 최대값과 right의 최소값) 사이에 값을 고른다
			startX = endX = uniform_int_distribution<size_t>(max(cell1.left, cell2.left), max(cell1.right, cell2.right))(_engine);
		}

		if (cell1.top > cell2.bottom)//cell1이 cell2의 위에 있다.
		{
			startY = cell2.bottom;
			endY = cell1.top;
		}
		else if (cell2.top > cell1.bottom)//cell2가 cell1의 위에 있다.
		{
			startY = cell1.bottom;
			endY = cell2.top;
		}
		else//두 사각형의 Y축에서 겹치는 부분이 있다.
		{
			//겹치는 부분(top의 최대값과 bottom의 최소값) 사이에 값을 고른다.
			startY = endY = uniform_int_distribution<size_t>(max(cell1.top, cell2.top), max(cell1.bottom, cell2.bottom))(_engine);
		}
		

		result.splice(result.begin(), cellList1);
		result.splice(result.begin(), cellList2);

		return result;
	}
};

//BSP를 사용한 맵생성
//throw : 맵 생성이 실패했을경우 logic_error, 맵변수나 랜덤시드가 잘못되었을경우 bad_argument
//return: this
//_treeData : BSPTree생성에 필요한 인자
//_cellData : BSPTree로 방을 만들때 필요한 인자
//_seed : 맵을 생성하기 위해 필요한 시드값(없으면 랜덤한 값으로 생성됨)
cMap& cMap::CreateBSPMap(const BSPTreeCreateData& _treeData, const BSPCellCreateData& _cellData, unsigned int _seed)
{
	std::mt19937_64 randEngine(_seed);

	BSPTree tree(randEngine, _treeData, BSPCellData(m_width, m_height));
	std::fill(m_map.begin(), m_map.end(), TILE_TYPE::WALL);
	auto result = tree.CreateMap(randEngine, *this, _cellData);

	return *this;
}