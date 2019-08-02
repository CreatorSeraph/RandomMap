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

//BSP�� ����� �ʻ���
//throw : �� ������ ����������� logic_error, �ʺ����� �����õ尡 �߸��Ǿ������ bad_argument
//return: this
//_deep : ���� ����
//_seed : ���� �����ϱ� ���� �ʿ��� �õ尪(������ ������ ������ ������)
cMap& cMap::CreateBSPMap(size_t _deep, unsigned int _seed)
{
	std::mt19937_64 randEngine(_seed);
	return *this;
}