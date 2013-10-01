#include <Frustum.hpp>

using namespace GlUtils;

//----------------------------------------------------------------------------------------
Frustum::Frustum()
    : fieldOfViewY(0.0f),
      aspectRatio(0.0f),
      zNear(0.0f),
      zFar(0.0f),
      _isPerspective(false),
      recalcPerspectiveMatrix(false) {

}

//----------------------------------------------------------------------------------------
/**
 * Constructs an orthographic projection view \c Frustum.
 *
 * @note If zNear or zFar are negative, corresponding z clipping planes
 * are considered behind the viewer.
 *
 * @param left - specify location of left clipping plane.
 * @param right - specify location of right clipping plane.
 * @param bottom - specify location of bottom clipping plane.
 * @param top - specify location of top clipping plane.
 * @param zNear - distance to near z clipping plane.
 * @param zFar - distance to far z clipping plane.
 */
Frustum::Frustum(float left, float right, float bottom, float top, float zNear, float zFar)
        : fieldOfViewY(0.0f),
          aspectRatio((right - left) / (top - bottom)),
          zNear(zNear),
          zFar(zFar),
          _isPerspective(false),
          recalcPerspectiveMatrix(false) {

    projectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
}

//----------------------------------------------------------------------------------------
/**
 * Constructs a perspective projection view \c Frustum.
 *
 * @note  Depth buffer precision is affected by the values specified for
 * zNear and zFar. The greater the ratio of zFar to zNear is, the less
 * effective the depth buffer will be at distinguishing between surfaces
 * that are near each other. If  r = zFar zNear roughly log2(r) bits
 * of depth buffer precision are lost. Because r approaches infinity as
 * zNear approaches 0, zNear must never be set to 0.
 *
 * @param fieldOfViewY
 * @param aspectRatio
 * @param zNear - distance to near z clipping plane (always positive).
 * @param zFar - distance to far z clipping plane (always positive).
 */
Frustum::Frustum(float fieldOfViewY, float aspectRatio, float zNear, float zFar)
        : fieldOfViewY(fieldOfViewY),
          aspectRatio(aspectRatio),
          zNear(zNear),
          zFar(zFar),
          _isPerspective(true),
          recalcPerspectiveMatrix(false) {

    projectionMatrix = glm::perspective(fieldOfViewY, aspectRatio, zNear, zFar);
}

//----------------------------------------------------------------------------------------
mat4 Frustum::getProjectionMatrix() const {
    if (recalcPerspectiveMatrix) {
        projectionMatrix = glm::perspective(fieldOfViewY, aspectRatio, zNear, zFar);
        recalcPerspectiveMatrix = false;
    }
    return projectionMatrix;
}

//----------------------------------------------------------------------------------------
float Frustum::getFieldOfViewY() const {
    return fieldOfViewY;
}

//----------------------------------------------------------------------------------------
float Frustum::getAspectRatio() const {
    return aspectRatio;
}

//----------------------------------------------------------------------------------------
float Frustum::getNearZDistance() const {
    return zNear;
}

//----------------------------------------------------------------------------------------
float Frustum::getFarZDistance() const {
    return zFar;
}

//----------------------------------------------------------------------------------------
bool Frustum::isPerspective() const {
    return _isPerspective;
}

//----------------------------------------------------------------------------------------
bool Frustum::isOrthographic() const {
    return !_isPerspective;
}

//----------------------------------------------------------------------------------------
void Frustum::setFieldOfViewY(float fieldOfViewY) {
    this->fieldOfViewY = fieldOfViewY;
    recalcPerspectiveMatrix = true;
}

//----------------------------------------------------------------------------------------
void Frustum::setAspectRatio(float aspectRatio) {
    this->aspectRatio = aspectRatio;
    recalcPerspectiveMatrix = true;
}

//----------------------------------------------------------------------------------------
void Frustum::setNearZDistance(float zNear) {
    this->zNear = zNear;
    recalcPerspectiveMatrix = true;
}

//----------------------------------------------------------------------------------------
void Frustum::setFarZDistance(float zFar) {
    this->zFar = zFar;
    recalcPerspectiveMatrix = true;
}
