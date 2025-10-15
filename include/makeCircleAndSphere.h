#pragma once
#include <vector>
struct Vertex {
  float x, y, z;    // position
  float nx, ny, nz; // normal
};
std::vector<float> makeCircle(float cx, float cy, float r, int N);
std::vector<Vertex> makeSphere(float radius, int sectorCount, int stackCount);
std::vector<unsigned int> makeSphereIndices(int sectorCount, int stackCount);
