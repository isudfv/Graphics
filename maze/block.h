#pragma once
class block {
public:
	block();
	//block(int x, int y);
	~block();
	std::tuple<int, int, int, int>findOverlapLine(const auto& other) const {
		if (x0 == other.x1) return {x0, y0, other.x1, other.y1};
		if (x1 == other.x0) return {x1, y1, other.x0, other.y0};
		if (y0 == other.y1) return {x0, y0, other.x1, other.y1};
		if (y1 == other.y0) return {x1, y1, other.x0, other.y0};
	}

public:
	int x, y;
	int x0, y0, x1, y1;
	bool colored;
};

block::block() : colored(0) {}

//inline block::block(int x, int y): x(x), y(y) {}

block::~block() {}