#ifndef SPLINE_H
#define SPLINE_H

#include <vector>
#include <utility>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

/**
* A class that represents the points on a spline.
*/
class SplinePoint
{
 public:
  /**
   * Default Constructor.
   */
  SplinePoint();

  /**
   * Copy constructor.
   * @param instance The instance that we want to copy.
   */
  SplinePoint(const SplinePoint& instance);

  /**
   * Sets the time/point pair of the SplinePoint.
   * @param time The time after which the point is reached.
   * @param point The point which is reached after the given time.
   * @return void Nothing.
   */
  void setPoint(float time, const glm::vec3& point);

  /**
   * Sets the tangent of this SplinePoint.
   * @param tangent The tangent to set.
   * @return void Nothing.
   */
  void setTangent(const glm::vec3& tangent);

  /**
   * Gets the point of the SplinePoint.
   * @return const glm::vec3& The point.
   */
  const glm::vec3& getPoint() const;

  /**
   * Gets the time at which the point is reached.
   * @return float The time.
   */
  float getTime() const;

  /**
   * Gets the tangent on the point.
   * @return const glm::vec3& The tangent.
   */
  const glm::vec3& getTangent() const;

  /**
   * Static compare function that compares to points of a spline.
   * @param a The first time/point pair that should be compared.
   * @param b The second time/point pair that should be compared.
   * @return bool Whether the first value should be before the second value
   *  when sorted.
   */
  static bool compare(SplinePoint a, SplinePoint b);

 private:
  /**
   * The time after which the point is reached.
   */
  float time_;

  /**
   * The point which is reached after the given time.
   */
  glm::vec3 point_;

  /**
   * The tangent of the point.
   */
  glm::vec3 tangent_;
};

/**
* Class that represents the results of a spline interplation.
*/
class SplineInterpolationResult
{
 public:
  /**
   * Constructor that initializes the position and orientation member.
   * @param position The position.
   * @param orientation The orientation.
   */
  SplineInterpolationResult(const glm::vec3& position,
      const glm::quat& orientation);

  /**
   * Copy constructor.
   * @param instance The instance that we want to copy.
   */
  SplineInterpolationResult(const SplineInterpolationResult& instance);

  /**
   * Get the interpolated position.
   * @return const glm::vec3& The interpolated position.
   */
  const glm::vec3& getPosition();

  /**
   * Get the interpolated orientation.
   * @return const glm::vec3& The interpolated orientation.
   */
  const glm::quat& getOrientation();

 private:
  /**
   * The interpolated position.
   */
  glm::vec3 position_;

  /**
   * The interpolated orientation.
   */
  glm::quat orientation_;
};

/**
* A class that represents splines. Beneath the surface it uses Catmull-Rom
* splines.
*/
class Spline
{
 public:
  /**
   * Default Constructor
   */
  Spline();

  /**
   * Copy constructor.
   * @param instance The instance that we want to copy.
   */
  Spline(const Spline& instance);

  /**
   * Adds a time/point pair to the spline.
   * @param time The time at which the interpolation reaches the point.
   * @param point The point which is reached by the interpolation at the
   *     passed time.
   * @return void Nothing
   */
  void addPoint(float time, const glm::vec3& point);

  /**
   * Calculates the SplinePoints tangent vectors and stores them.
   * @return void Nothing
   */
  void calculateAndStoreTangents();

  /**
   * Interpolates over the spline and calculates the position after a passed
   * time.
   * @param time The time for which we follow the spline after we reach the
   * searched position.
   * @return SplineInterpolationResult The interpolated position and
   * orientation.
   */
  SplineInterpolationResult interpolate(float time) const;

  /**
   * Returns the time of the first point.
   * @return float Time of the first point. 
   */
  float getStartTime() const;

  /**
   * Returns the time of the last point.
   * @return float Time of the last point. 
   */
  float getEndTime() const;

 private:
  /**
   * Stores the time/point/tangent pairs of the spline.
   */
  std::vector<SplinePoint> points_;

  /**
   * Calculates an orientation that rotates an object so that it looks to the
   * point specified by the look_at parameter.
   * @param look_at_direction The direction in which the object should look.
   * @return glm::quat An orientation that rotates an object to look in the
   * direction specified by look_at_direction.
   */
  glm::quat lookAtToOrientation(const glm::vec3& look_at_direction) const;
};

#endif
