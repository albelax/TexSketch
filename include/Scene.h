#ifndef SCENE_H
#define SCENE_H

#include "Shader.h"
#include "Mesh.h"
#include "Image.h"
#include "TrackballCamera.h"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <ext.hpp>
#include <glm.hpp>
#include <QOpenGLWidget>

class Scene : public QOpenGLWidget
{
  Q_OBJECT
public:
  Scene(QWidget *_parent );
  Scene(QWidget *_parent, Image * _image );
  ~Scene();
  virtual void mouseMove( QMouseEvent * _event ) = 0;
  virtual void mouseClick( QMouseEvent * _event ) = 0;
  void loadImage(); // = 0;
  virtual void calculateNormals( int _depth, bool _invert ) = 0;
  virtual void calculateSpecular( int _brightness, int _contrast, bool _invert, int _sharpness, bool _equalize ) = 0;
  virtual void calculateRoughness( int _brightness, int _contrast, bool _invert, int _sharpness, bool _equalize ) = 0;
  virtual void calculateMetallic(int _x, int _y, float _range ) = 0;
  virtual void calculateDiffuse(int _brightness, int _contrast, int _sharpness) = 0;
  virtual void calculateAO(int _depth, int _contrast, int _brightness) = 0;
  virtual void calculateDisplacement( int _brightness, int _contrast, bool _invert, int _sharpness, bool _equalize ) = 0;
  void toggleMetallic(bool _b);
  void toggleAO(bool _b);
  void setUpdate( bool _canUpdate ) { m_canUpdate = _canUpdate; }
  bool canUpdate() { return m_canUpdate; }
protected:
  bool m_canUpdate;
  GLuint m_vao;
  GLuint m_nbo;
  GLuint m_vbo;
  GLuint m_tbo;
  GLint m_MVAddress;
  GLint m_MVPAddress;
  GLint m_NAddress;
  Image * m_editedImage;
  Shader m_shader;
  Mesh m_mesh;
  glm::mat4 m_view;
  glm::mat4 m_projection;
  glm::mat4 m_MV;
  glm::mat4 m_MVP;
  TrackballCamera m_camera;
  std::vector<GLuint> m_textures;
};

#endif // SCENE_H
