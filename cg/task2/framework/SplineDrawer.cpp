#include "SplineDrawer.h"
#include "GLBuffer.h"
#include "Spline.h"
#include "Shader.h"

SplineDrawer::SplineDrawer(const Spline& spline, Shader& shader)
    : spline_(spline)
    , vbo_(GLBuffer::BufferType::VERTEX_BUFFER)
    , shader_(shader)
    , point_count_(0)
    , tangent_count_(0)
{
}

void SplineDrawer::init()
{
  std::vector<glm::vec3> points, tangents;
  calculateInterpolatedPointsAndTangents(points, tangents);
  fillVBO(points, tangents);
}

void SplineDrawer::draw()
{
  vbo_.bind();

  // Draw the points.
  shader_.setAttribPointer("position", GL_FLOAT, 3, 0);
  shader_.enableAttribArray("position");
  shader_.setUniformMatrix4f("model_mat", glm::mat4(1));
  shader_.setUniform3f("mat_diffuse", glm::vec3(0.5f, 1.0f, 0.f));
  glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(point_count_));

  // Draw the tangents.
  shader_.setUniform3f("mat_diffuse", glm::vec3(0.2f, 0.f, 0.f));
  glDrawArrays(GL_LINES, static_cast<GLint>(point_count_), static_cast<GLsizei>(tangent_count_));

  vbo_.unbind();
}

void SplineDrawer::calculateInterpolatedPointsAndTangents(
    std::vector<glm::vec3>& out_points,
    std::vector<glm::vec3>& out_tangents)
{
  out_points.clear();
  out_tangents.clear();
  for (float spline_time = spline_.getStartTime();
       spline_time < spline_.getEndTime(); spline_time += 0.5f)
  {
    SplineInterpolationResult interplation_result =
        spline_.interpolate(spline_time);
    out_points.push_back(interplation_result.getPosition());

    out_tangents.push_back(out_points.back());
    out_tangents.push_back(out_points.back() +
        interplation_result.getOrientation() * glm::vec3(0, 0, 1));
  }
  point_count_ = out_points.size();
  tangent_count_ = out_tangents.size();
}

void SplineDrawer::fillVBO(const std::vector<glm::vec3>& points,
    const std::vector<glm::vec3>& tangents)
{
  vbo_.create();
  vbo_.bind();
  size_t byte_count_points = points.size() * sizeof(glm::vec3);
  size_t byte_count_tangents = tangents.size() * sizeof(glm::vec3);
  size_t byte_count = byte_count_points + byte_count_tangents;
  vbo_.allocate(byte_count, GLBuffer::Usage::STATIC_DRAW);
  vbo_.write(points, 0);
  vbo_.write(tangents, static_cast<int>(byte_count_points));
  vbo_.unbind();
}
