#include <cmath>
#include <vector>

// ╭────────╮
// │ Circle │
// ╰────────╯
std::vector<float> makeCircle(float cx, float cy, float r, int N) {
  std::vector<float> vertices;
  vertices.push_back(cx);
  vertices.push_back(cy);
  for (int i{0}; i <= N; ++i) {
    float x = cx + r * cos((float)2 * M_PI * (float)i / (float)N);
    float y = cy + r * sin((float)2 * M_PI * (float)i / (float)N);
    vertices.push_back(x);
    vertices.push_back(y);
  }
  return vertices;
}
// ╭────────╮
// │ Sphere │
// ╰────────╯
struct Vertex {
  float x, y, z;    // position
  float nx, ny, nz; // normal
};
std::vector<Vertex> makeSphere(float radius, int sectorCount, int stackCount) {
  std::vector<Vertex> vertices;

  float pi = M_PI;
  float twoPi = 2.0f * M_PI;

  for (int i = 0; i <= stackCount; ++i) {
    float stackAngle = pi / 2 - i * (pi / stackCount); // from pi/2 to -pi/2
    float xy = radius * cosf(stackAngle);
    float z = radius * sinf(stackAngle);

    for (int j = 0; j <= sectorCount; ++j) {
      float sectorAngle = j * (twoPi / sectorCount); // 0 to 2pi

      float x = xy * cosf(sectorAngle);
      float y = xy * sinf(sectorAngle);

      // normalize for normal
      float nx = x / radius;
      float ny = y / radius;
      float nz = z / radius;

      vertices.push_back({x, y, z, nx, ny, nz});
    }
  }
  return vertices;
}
std::vector<unsigned int> makeSphereIndices(int sectorCount, int stackCount) {
  std::vector<unsigned int> indices;
  int k1, k2;

  for (int i = 0; i < stackCount; ++i) {
    k1 = i * (sectorCount + 1); // beginning of current stack
    k2 = k1 + sectorCount + 1;  // beginning of next stack

    for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
      if (i != 0) {
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k1 + 1);
      }

      if (i != (stackCount - 1)) {
        indices.push_back(k1 + 1);
        indices.push_back(k2);
        indices.push_back(k2 + 1);
      }
    }
  }
  return indices;
}
