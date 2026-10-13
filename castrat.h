#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <limits>
#include <algorithm>


namespace stdPather {
	enum TerrainType : i_1b {
		WALKABLE = '.',
		BLOKABLE = 0,
		SLOWABLE = 'Y'
	};


	struct Point {
		int x, y;

		Point(int x = 0, int y = 0) : x(x), y(y) {}

		bool operator==(const Point& other) const {
			return x == other.x && y == other.y;
		}

		bool operator!=(const Point& other) const {
			return !(*this == other);
		}
	};

	// Хеш-функция для Point (для использования в unordered_map)
	struct PointHash {
		std::size_t operator()(const Point& p) const {
			return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
		}
	};

	struct Node {
		Point point;
		int g;          // стоимость от старта
		int h;          // эвристика до цели
		int f;          // g + h
		Point parent;
		bool has_parent;

		Node(Point p, int g, int h, Point parent = Point(), bool has_parent = false)
			: point(p), g(g), h(h), f(g + h), parent(parent), has_parent(has_parent) {
		}

		// Для priority_queue (min-heap)
		bool operator>(const Node& other) const {
			return f > other.f;
		}
	};

	class AStar {
	private:
		static const int MAP_SIZE = 256;
		int8_t(*map)[MAP_SIZE];

		// Проверка, что координаты в пределах карты и точка проходима
		bool isValid(int x, int y) {
			return
				x >= 0 && x < MAP_SIZE &&
				y >= 0 && y < MAP_SIZE &&
				map[x][y] != 0;// TerrainType::BLOKABLE;
		}

		// Эвристика - манхэттенское расстояние
		int heuristic(Point a, Point b) {
			return std::abs(a.x - b.x) + std::abs(a.y - b.y);
		}

		// Получить соседей (4 направления)
		std::vector<Point> getNeighbors(Point p) {
			std::vector<Point> neighbors;
			int dx[] = { -1, 1, 0, 0 };
			int dy[] = { 0, 0, -1, 1 };

			for (int i = 0; i < 4; ++i) {
				int nx = p.x + dx[i];
				int ny = p.y + dy[i];
				if (isValid(nx, ny)) {
					neighbors.emplace_back(nx, ny);
				}
			}
			return neighbors;
		}

	public:
		AStar(int8_t(*m)[MAP_SIZE]) : map(m) {}

		// Возвращает путь от start до goal (включая обе точки)
		// Если путь не найден, возвращает пустой вектор
		void findPath(std::vector<Point>* path_this, Point start, Point goal) {
			// Проверка валидности начальной и конечной точек
			if (!isValid(start.x, start.y) || !isValid(goal.x, goal.y)) return;

			// Если старт == цель
			if (start == goal) {
				path_this->push_back(start);
				return;
			}

			// Очередь с приоритетом для открытого списка
			std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;

			// Храним лучшие g-значения для каждой точки
			std::unordered_map<Point, int, PointHash> gScore;

			// Храним родителей для восстановления пути
			std::unordered_map<Point, Point, PointHash> cameFrom;

			// Инициализация
			openSet.emplace(start, 0, heuristic(start, goal));
			gScore[start] = 0;

			while (!openSet.empty()) {
				Node current = openSet.top();
				openSet.pop();

				Point currentPoint = current.point;

				// Достигли цели - восстанавливаем путь
				if (currentPoint == goal) {
					std::vector<Point>& path = *path_this;
					Point p = goal;

					while (p != start) {
						path.push_back(p);
						p = cameFrom[p];
					}
					path.push_back(start);

					std::reverse(path.begin(), path.end());
					return;
				}

				// Пропускаем устаревшие записи
				if (current.g > gScore[currentPoint]) {
					continue;
				}

				// Исследуем соседей
				for (const Point& neighbor : getNeighbors(currentPoint)) {
					int tentativeG = current.g + 1; // Стоимость перехода = 1

					auto it = gScore.find(neighbor);
					if (it == gScore.end() || tentativeG < it->second) {
						// Нашли лучший путь к соседу
						gScore[neighbor] = tentativeG;
						cameFrom[neighbor] = currentPoint;

						int h = heuristic(neighbor, goal);
						openSet.emplace(neighbor, tentativeG, h, currentPoint, true);
					}
				}
			}
		}
	};
}