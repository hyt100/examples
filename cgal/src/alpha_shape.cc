// 注1： cgal是一个头文件库，编译对C++版本有要求，且alpha-shape需要链接gmp库
// 注2： 测试发现，CGAL实现对内凹的角效果不佳，内凹的角会被去除掉。

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Alpha_shape_2.h>
#include <CGAL/Alpha_shape_vertex_base_2.h>
#include <CGAL/Alpha_shape_face_base_2.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/algorithm.h>
#include <CGAL/assertions.h>
#include <vector>
#include <iostream>
#include <cmath>
#include "Eigen"
#include "util.h"
typedef CGAL::Exact_predicates_inexact_constructions_kernel  K;
typedef K::FT                                                FT;
typedef K::Point_2                                           Point;
typedef K::Segment_2                                         Segment;
typedef CGAL::Alpha_shape_vertex_base_2<K>                   Vb;
typedef CGAL::Alpha_shape_face_base_2<K>                     Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>          Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                Triangulation_2;
typedef CGAL::Alpha_shape_2<Triangulation_2>                 Alpha_shape_2;
typedef Alpha_shape_2::Alpha_shape_edges_iterator            Alpha_shape_edges_iterator;
template <class OutputIterator>
void alpha_edges( const Alpha_shape_2& A, OutputIterator out)
{
  Alpha_shape_edges_iterator it = A.alpha_shape_edges_begin(),
                             end = A.alpha_shape_edges_end();
  for( ; it!=end; ++it)
    *out++ = A.segment(*it);
}

static bool FindShape(std::vector<Segment> segments_in, std::vector<Segment> &segments_out)
{
  int num = (int)segments_in.size();
  if (num < 3) {
    std::cout << "FindShape: too few segments" << std::endl;
    return false;
  }
  segments_out.resize(num);
  bool first = true;

  for (int i = 0; i < num; ++i) {
    Segment segment;
    int j = 0;
    while (j < (int)segments_in.size()) {
      segment = segments_in[j];

      // first segment
      if (first) {
        first = false;
        break;
      }

      // others
      double expect_x = segments_out[i-1].vertex(1).x();
      double expect_y = segments_out[i-1].vertex(1).y();
      // std::cout << "FindShape: expect is " << expect_x << "," << expect_y << std::endl;
      if (FloatEQ(expect_x, segments_in[j].vertex(0).x()) && FloatEQ(expect_y, segments_in[j].vertex(0).y())) 
      {
        break;
      }
      if (FloatEQ(expect_x, segments_in[j].vertex(1).x()) && FloatEQ(expect_y, segments_in[j].vertex(1).y()))
      {
        segment = Segment(Point(segments_in[j].vertex(1).x(), segments_in[j].vertex(1).y()),
                          Point(segments_in[j].vertex(0).x(), segments_in[j].vertex(0).y()));
        break;
      }

      ++j;
    }
    if (j >= segments_in.size()) {
      std::cout << "FindShape: not found segment. i=" << i << std::endl;
      return false;
    }

    // std::cout << "FindShape: ======" << std::endl;
    
    segments_out[i] = segment;
    segments_in.erase(segments_in.begin() + j);
  }

  if (FloatEQ(segments_out[0].vertex(0).x(), segments_out[num-1].vertex(1).x()) && 
      FloatEQ(segments_out[0].vertex(0).y(), segments_out[num-1].vertex(1).y()))
  {
    return true;
  }

  std::cout << "FindShape: no closure" << std::endl;
  return false;
}

bool AlphaShape(std::vector<Eigen::Vector2f> &points_in, std::vector<Eigen::Vector2f> &points_out, float alpha)
{
  // copy data
  std::vector<Point> points;
  for (auto &p_in: points_in) {
    Point p(p_in.x(), p_in.y());
    points.emplace_back(p);
  }

  // alpha-shape compute
  Alpha_shape_2 A(points.begin(), points.end(),
                  FT(alpha),
                  Alpha_shape_2::GENERAL);
  std::vector<Segment> segments;
  alpha_edges(A, std::back_inserter(segments));
  std::cout << "Alpha Shape computed" << std::endl;
  std::cout << segments.size() << " alpha shape edges" << std::endl;
  std::cout << "Optimal alpha: " << *A.find_optimal_alpha(1)<<std::endl;
  // for (auto &seg: segments) {
  //   std::cout << seg.vertex(0).x() << ", " << seg.vertex(0).y() << ",   " 
  //             << seg.vertex(1).x() << ", " << seg.vertex(1).y() << ",   " << std::endl;
  // }

  // find shape
  std::vector<Segment> segments_sorted;
  if (!FindShape(segments, segments_sorted)) {
    std::cout << "find shape failure" << std::endl;
    return false;
  }
  // std::cout << "find shape: " << std::endl;
  // for (auto &seg: segments_sorted) {
  //   std::cout << seg.vertex(0).x() << ", " << seg.vertex(0).y() << ",   " 
  //             << seg.vertex(1).x() << ", " << seg.vertex(1).y() << ",   " << std::endl;
  // }

  // // output vertex
  for (auto &seg: segments_sorted) {
    points_out.emplace_back(Eigen::Vector2f(seg.vertex(0).x(), seg.vertex(0).y()));
    points_out.emplace_back(Eigen::Vector2f(seg.vertex(1).x(), seg.vertex(1).y()));
  }
  return true;
}

int main()
{
  std::vector<Eigen::Vector2f> points_in = {
    {-7.81539, 4.51858},
    {-7.93202,  4.52168},
    {-7.94138,  7.58009},
    {-0.286818,  7.60267},
    {-0.280561,  3.88406},
    {-4.06186,  4.19632},
    {-3.96622,  1.23156},
    {-7.92189,  1.21184},
    {-7.93122,  4.26167},
    {-7.80361,  4.23233}
  };
  std::vector<Eigen::Vector2f> points_out;

  AlphaShape(points_in, points_out, 20.0f);
  return 0;
}
