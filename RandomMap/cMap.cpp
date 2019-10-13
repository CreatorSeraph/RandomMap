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
		_out << (iter == TILE_TYPE::GROUND ? "��" : "��");
		if (++loopCount == mapWidth)
		{
			_out << endl;
			loopCount = 0;
		}
	}

	return _out;
}

//Noise�� ����� �ʻ���
//throw     : �� ������ ����������� logic_error, �ʺ����� �����õ尡 �߸��Ǿ������ bad_argument
//return    : this
//_loopCount: ���� �ٵ���� Ƚ��(�������� ���� �۰� ���������� ������ ����� ����� ����
//_seed     : ���� �����ϱ� ���� �ʿ��� �õ尪(������ ������ ������ ������)
//_wallCount: ������ �ٲ������ �ʿ��� ������ ���� ����
//_cellRatio: �ʱ�ĭ�� ���� Ȯ��(�������� ���� ���� �����ȴ�)
cMap& cMap::CreateNoiseMap(size_t _loopCount, unsigned int _seed, size_t _wallCount, float _cellRatio)
{
	constexpr bool printLog = false;

	//�õ尪���� ���������� ����
	mt19937_64 randEngine(_seed);

	auto& mapTile = m_map;
	const auto& mapWidth = m_width;
	const auto& mapHeight = m_height;
	auto GetIdx = [mapWidth](int y, int x) { return x + y * mapWidth; };//�ε����� �����ִ� �Լ�
	mapArrType prevMapTile(mapWidth * mapHeight);//�������� ���� ���� �ٵ������̱⶧���� ����ʰ� �Ȱ��� �������� ���� �ϳ� ������ش�.

	auto FindAdjacentGroundCount = [&prevMapTile, GetIdx, mapWidth, mapHeight](int y, int x) {//������ ���� ������ ���ϴ� �����Լ�
		size_t result = 0;
		int yTarget = min(y + 1, static_cast<int>(mapHeight) - 1);
		int xTarget = min(x + 1, static_cast<int>(mapWidth) - 1);//�� �ݺ��� �������� ����ϴ°ͺ��� �̸� ���Ȱ��� ���°��� ����.
		for (int i = max(y - 1, 0); i <= yTarget; ++i)
		{
			for (int j = max(x - 1, 0); j <= xTarget; ++j)
			{
				result += (prevMapTile[GetIdx(i, j)] == TILE_TYPE::GROUND);//bool������ ���������� ��ȯ�ɶ� false�� 0, true�� 1�� �ٲ��.
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
	}//�����ϰ� �ʱⰪ�� �������ش�.

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
template<class... Ts> overload(Ts...)->overload<Ts...>;//�����ε� ���� ���

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
		//������ 2��� ����ϴ� ������ float�� �߸��� �ֱ� ����. Ÿ�ϸ��̱⶧���� ���� float�ڷ����� ������� �ʴ´�.
		size_t xDistance2 = labs((left + right) - (_other.left + _other.right));//�� �簢���� x�� �Ÿ�(�� 2��)
		size_t widthSum = width() + _other.width();//�� �簢���� �ʺ��� ��(�� 2��)
		//xDistance <= widthSum / 2
		//xDistance * 2 <= widthSum
		if (xDistance2 < widthSum)//�� �簢���� �Ÿ���, �ʺ��� ���� ���ݺ��� Ŭ��� (���浹�˻��ϵ��� x�� �˻�)
			xDistance2 = widthSum;//�Ÿ��� ���ϴµ� ������ �Ÿ��� ���ü� ����.

		//(xDistnace - widthSum / 2)
		//(xDistnace2 - widthSum) / 2
		return (xDistance2 - widthSum) / 2;
	}

	size_t GetYDistance(const BSPCellData& _other)
	{
		size_t yDistance2 = labs((top + bottom) - (_other.top + _other.bottom));
		size_t heightSum = height() + _other.height();
		if (yDistance2 <= heightSum)//�ݺ�
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
		: m_data(_mapData)//���� ����� ���� ������ �Ѱ��ش�.
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

		//�̹� �ִ� ���̿� �����߰ų� ���� ������ �ʺ� ���� ���
		if (_deep == _createData.deep || (xDivVal == -1 && yDivVal == -1))
		{
			m_data = std::move(_mapData);//���� ����� ���� ������ �Ѱ��ش�.
			cout << "������ ���" << endl;
			return;
		}

		m_isDivWidth = std::bernoulli_distribution(static_cast<float>(xDivVal + 1) / (xDivVal + yDivVal + 2))(_engine);//true = ���η� ����, false = ���η� ����
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
			//����� ������ �κ��̴�. �ʻ����� �ʿ���
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
			auto GetIdx = [mapWidth](int y, int x) { return x + y * mapWidth; };//�ε����� �����ִ� �Լ�

			for (size_t i = data.left; i <= data.right; ++i)
			{
				for (size_t j = data.top; j <= data.bottom; ++j)
				{
					_map[GetIdx(j, i)] = TILE_TYPE::GROUND;
				}
			}

			//���� ���� ũ�⸦ ��ȯ�Ͽ� ���� �����ų�� ������ �ǵ��� �Ѵ�.
			return { data };
		}
		//�ڽĵ鿡�� ���� �����ϰ� ��Ų��
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
		//���� �Ÿ��� ª�� �ΰ��� cell�� ���Ѵ�

		int startX, startY;
		int endX, endY;
		if (cell1.left > cell2.right)//cell1�� cell2�� �����ʿ� �ִ�.
		{
			startX = cell2.right;
			endX = cell1.left;
		}
		else if (cell2.left > cell1.right)//cell2�� cell1�� �����ʿ� �ִ�.
		{
			startX = cell1.right;
			endX = cell2.left;
		}
		else//�� �簢���� X�࿡�� ��ġ�� �κ��� �ִ�.
		{
			//��ġ�� �κ�(left�� �ִ밪�� right�� �ּҰ�) ���̿� ���� ����
			startX = endX = uniform_int_distribution<size_t>(max(cell1.left, cell2.left), max(cell1.right, cell2.right))(_engine);
		}

		if (cell1.top > cell2.bottom)//cell1�� cell2�� ���� �ִ�.
		{
			startY = cell2.bottom;
			endY = cell1.top;
		}
		else if (cell2.top > cell1.bottom)//cell2�� cell1�� ���� �ִ�.
		{
			startY = cell1.bottom;
			endY = cell2.top;
		}
		else//�� �簢���� Y�࿡�� ��ġ�� �κ��� �ִ�.
		{
			//��ġ�� �κ�(top�� �ִ밪�� bottom�� �ּҰ�) ���̿� ���� ����.
			startY = endY = uniform_int_distribution<size_t>(max(cell1.top, cell2.top), max(cell1.bottom, cell2.bottom))(_engine);
		}
		

		result.splice(result.begin(), cellList1);
		result.splice(result.begin(), cellList2);

		return result;
	}
};

//BSP�� ����� �ʻ���
//throw : �� ������ ����������� logic_error, �ʺ����� �����õ尡 �߸��Ǿ������ bad_argument
//return: this
//_treeData : BSPTree������ �ʿ��� ����
//_cellData : BSPTree�� ���� ���鶧 �ʿ��� ����
//_seed : ���� �����ϱ� ���� �ʿ��� �õ尪(������ ������ ������ ������)
cMap& cMap::CreateBSPMap(const BSPTreeCreateData& _treeData, const BSPCellCreateData& _cellData, unsigned int _seed)
{
	std::mt19937_64 randEngine(_seed);

	BSPTree tree(randEngine, _treeData, BSPCellData(m_width, m_height));
	std::fill(m_map.begin(), m_map.end(), TILE_TYPE::WALL);
	auto result = tree.CreateMap(randEngine, *this, _cellData);

	return *this;
}