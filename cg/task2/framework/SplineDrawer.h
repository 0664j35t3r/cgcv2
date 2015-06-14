#ifndef SPLINEDRAWER_H
#define SPLINEDRAWER_H

#include "Spline.h"
#include "GLBuffer.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <vector>

/**
* This class is responsible for drawing splines.
*/
class SplineDrawer
{
 public:
  /**
   * Constructor that initializes the spline reference.
   * @param spline A reference to the spline that is to be drawn.
   * @param shader A reference to the shader that is used.
   */
  SplineDrawer(const Spline& spline, Shader& shader);

  /**
   * Initializes everything needed for the drawing.
   * @return void Nothing. 
   */
  void init();

  /**
   * Draws the spline.
   * @return void Nothing. 
   */
  void draw();

 private:
  /**
   * Copy constructor.
   * @param instance The instance that we want to copy.
   */
  SplineDrawer(const SplineDrawer& instance);

  /**
   * Description
   * @param out_points The interpolated points will be stored here.
   * @param out_tangents The interpolated tangents will be stored here.
   * @return void Nothing.
   */
  void calculateInterpolatedPointsAndTangents(
      std::vector<glm::vec3>& out_points,
      std::vector<glm::vec3>& out_tangents);

  /**
   * Fills the vbo with the necessary data to draw the points and the tangents.
   * @param points The points to draw.
   * @param tangents The tangents to draw.
   * @return void Nothing.
   */
  void fillVBO(const std::vector<glm::vec3>& points,
      const std::vector<glm::vec3>& tangents);

  /**
   * A reference to the spline that is to be drawn.
   */
  const Spline& spline_;

  /**
   * The vertex buffer object that will contain the splines points and tangents
   * to draw.
   */
  GLBuffer vbo_;

  /**
   * A reference to the shader that is used to draw.
   */
  Shader& shader_;

  /**
   * The number of spline points that are to be drawn.
   */
  size_t point_count_;

  /**
   * The number of spline tangents that are to be drawn.
   */
  size_t tangent_count_;
};

#endif
