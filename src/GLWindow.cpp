#include "GLWindow.h"

#include <iostream>
#include <QColorDialog>
#include <QGLWidget>
#include <QImage>
#include <QColor>
#include <math.h>

//----------------------------------------------------------------------------------------------------------------------

GLWindow::GLWindow( QWidget *_parent ) : QOpenGLWidget( _parent )
{

    m_plane = Mesh( "models/plane.obj", "plane" );
    this->resize(_parent->size());
    m_camera.setInitialMousePos(0,0);
    m_camera.setTarget(0.0f, 0.0f, -2.0f);
    m_camera.setEye(0.0f, 0.0f, 0.0f);
}

//----------------------------------------------------------------------------------------------------------------------

void GLWindow::initializeGL()
{
#ifdef linux
    // this needs to be after the context creation, otherwise it GLEW will crash
    //std::cout <<"linux \n";
    glewExperimental = GL_TRUE;
    glewInit();
    //	GLenum error = glGetError();
#endif
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_MULTISAMPLE );
    glEnable( GL_TEXTURE_2D );
    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
    glViewport( 0, 0, devicePixelRatio(), devicePixelRatio() );
    init();
}

//----------------------------------------------------------------------------------------------------------------------

void GLWindow::resizeGL( int _w, int _h )
{

}

//----------------------------------------------------------------------------------------------------------------------

GLWindow::~GLWindow()
{

}

//----------------------------------------------------------------------------------------------------------------------

void GLWindow::mouseMove(QMouseEvent * _event)
{
    m_camera.handleMouseMove( _event->pos().x(), _event->pos().y() );
    update();
}

//----------------------------------------------------------------------------------------------------------------------

void GLWindow::mouseClick(QMouseEvent * _event)
{
    m_camera.handleMouseClick(_event->pos().x(), _event->pos().y(), _event->type(), _event, 0);
    update();
}

//----------------------------------------------------------------------------------------------------------------------

void GLWindow::addTexture(std::string _image)
{
    GLuint tmp;
    m_textures.push_back(tmp);
    glActiveTexture( GL_TEXTURE0 + ( m_textures.size() - 1 ) );
    glGenTextures( 1, &m_textures[ m_textures.size() - 1 ] );

    m_image.load(_image.c_str());

    m_glImage = QGLWidget::convertToGLFormat(m_image);
    if(m_glImage.isNull())
        qWarning("IMAGE IS NULL");
    glBindTexture( GL_TEXTURE_2D, m_textures[m_textures.size()-1] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_glImage.width(), m_glImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_glImage.bits() );
}

//----------------------------------------------------------------------------------------------------------------------

void GLWindow::init()
{
    //	std::cerr << "OpenGL Version :" << glGetString(GL_VERSION) << std::endl;
    std::string shadersAddress = "shaders/";
    m_renderShader = Shader( "m_toScreen", shadersAddress + "renderedVert.glsl", shadersAddress + "renderedFrag.glsl" );
    glLinkProgram( m_renderShader.getShaderProgram() );

    glGenVertexArrays( 1, &m_vao );
    glBindVertexArray( m_vao );
    glGenBuffers( 1, &m_vbo );
    glGenBuffers( 1, &m_tbo );

    int amountVertexData = m_plane.getAmountVertexData() ;

    m_plane.setBufferIndex( 0 );
    // load vertices
    glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
    glBufferData( GL_ARRAY_BUFFER, amountVertexData * sizeof(float), 0, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, m_plane.getBufferIndex() * sizeof( float ) , m_plane.getAmountVertexData() * sizeof( float ), &m_plane.getVertexData() );
    // pass vertices to shader

    // load texture coordinates
    glBindBuffer( GL_ARRAY_BUFFER,	m_tbo );
    glBufferData( GL_ARRAY_BUFFER, amountVertexData * sizeof(float), 0, GL_STATIC_DRAW) ;
    glBufferSubData( GL_ARRAY_BUFFER, m_plane.getBufferIndex()/3*2 * sizeof( float ), m_plane.getAmountVertexData() * sizeof(float), &m_plane.getUVsData() );

    addTexture( "images/sky_xneg.png" );
    //    glViewport( 0, 0, m_image.width()/m_image.height(), m_image.width()/m_image.height() );
    //threshold();
    std::vector<std::vector<float>> intensityImage = intensity();
    std::vector<std::vector<std::vector<float>>> chromaImage = chroma( intensityImage );

    separation(intensityImage,chromaImage);

    saveImage1f(intensityImage, "images/grayscale.jpg" );

    glUniform1i( m_colourTextureAddress, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glGenerateMipmap( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE0 );



}

//------------------------------------------------------------------------------------------------------------------------------

void GLWindow::paintGL()
{
    glViewport( 0, 0, width(), height() );
    glClearColor( 1, 1, 1, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    renderTexture();

    update();
}

//------------------------------------------------------------------------------------------------------------------------------

void GLWindow::renderScene()
{

}

//------------------------------------------------------------------------------------------------------------------------------

void GLWindow::renderTexture()
{
    glViewport( 0, 0, width(), height() );
    glClearColor( 1, 1, 1, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glBindTexture( GL_TEXTURE_2D, m_textures[0]);

    glUniform1i( m_colourTextureAddress, 0 );

    glActiveTexture( GL_TEXTURE0 );

    glUseProgram( m_renderShader.getShaderProgram() );

    glBindVertexArray( m_vao );
    glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
    glEnableVertexAttribArray( glGetAttribLocation( m_renderShader.getShaderProgram(), "VertexPosition" ) );
    glVertexAttribPointer( glGetAttribLocation( m_renderShader.getShaderProgram(), "VertexPosition" ), 3, GL_FLOAT, GL_FALSE, 0, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, m_tbo );
    glEnableVertexAttribArray( glGetAttribLocation( m_renderShader.getShaderProgram(), "TexCoord" ) );
    glVertexAttribPointer( glGetAttribLocation( m_renderShader.getShaderProgram(), "TexCoord" ), 2, GL_FLOAT, GL_FALSE, 0, (void*) 0 );

    glDrawArrays( GL_TRIANGLES, m_plane.getBufferIndex() / 3, ( m_plane.getAmountVertexData() / 3 ) );
}

//------------------------------------------------------------------------------------------------------------------------------

void GLWindow::threshold()
{
    for(int i = 0; i < m_image.width(); ++i)
    {
        for(int j = 0; j< m_image.height(); ++j)
        {
            QColor myPixel = QColor( m_image.pixel(i,j) );

            int gray = (myPixel.red()*11 + myPixel.green()*16 + myPixel.blue()*5)/32;
            if( gray>235 || gray<25 )
            {
                int r,g,b;
                float fraction = 235.0f/gray;
                r = myPixel.red() * fraction;
                g = myPixel.green() * fraction;
                b = myPixel.blue() * fraction;

                m_image.setPixel(i,j,qRgb(r,g,b));
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------

void GLWindow::saveImage3f( std::vector<std::vector<std::vector<float>>> & _image, std::string _destination )
{
    QImage out;
    out = m_image.copy();

    for ( int i = 0; i < _image.size(); ++i )
    {
        for (int j = 0; j < _image[i].size(); ++j )
        {
            float r = _image[i][j][0]*255.0f;
            float g = _image[i][j][1]*255.0f;
            float b = _image[i][j][2]*255.0f;
            out.setPixel(i,j,qRgb( r,g,b ));
        }
    }
    out.save( _destination.c_str(), 0, -1 );
}

void GLWindow::saveImage1f( std::vector<std::vector<float>> & _image, std::string _destination )
{
    QImage out;
    out = m_image.copy();

    for ( int i = 0; i < (int)_image.size(); ++i )
    {
        for (int j = 0; j < (int)_image[i].size(); ++j )
        {
            float r = _image[i][j]*255.0f;
            float g = _image[i][j]*255.0f;
            float b = _image[i][j]*255.0f;
            out.setPixel(i,j,qRgb( r,g,b ));
        }
    }
    out.save( _destination.c_str(), 0, -1 );
}

//------------------------------------------------------------------------------------------------------------------------------

std::vector<std::vector<float>> GLWindow::intensity()
{
    std::vector<std::vector<float>> result;
    result.resize(m_image.width());
    for(int i =0; i<m_image.width(); ++i)
    {
        result[i].resize(m_image.height());
        for(int j=0 ; j<m_image.height();++j)
        {
            QColor myColor = m_image.pixelColor(i,j);
            float average = (myColor.redF() + myColor.greenF() + myColor.blueF()) / 3.0f;
            result[i][j] = average;
        }
    }
    return result;
}

//------------------------------------------------------------------------------------------------------------------------------

std::vector< std::vector< std::vector< float > > > GLWindow::chroma( std::vector<std::vector<float>> & _intensity )
{
    //    QImage result = m_image.copy();
    QColor color;
    std::vector<float> pixel;
    pixel.resize(3);
    std::vector<std::vector<std::vector<float>>> result; // 2d vector containing RGB Values
    result.resize( _intensity.size() );

    for ( int i = 0; i < _intensity.size(); ++i )
    {
        result[i].resize( _intensity[i].size() );
        for ( int j = 0; j < _intensity[i].size(); ++j )
        {
            float intensity = _intensity[i][j];
            float red = float(m_image.pixelColor(i,j).redF()) / float(intensity);
            float green = float(m_image.pixelColor(i,j).greenF()) / float(intensity);
            float blue = 3.0f - red - green; // HELP ????????????
            pixel[0] = red;
            pixel[1] = green;
            pixel[2] = blue;
            result[i][j] = pixel;
            //           result.push_back(pixel);
            //           color.setHsv(red, green, blue);
            //           result.setPixelColor( i, j, color );
        }
    }
    return result;
}

//------------------------------------------------------------------------------------------------------------------------------

void GLWindow::separation(std::vector<std::vector<float>> _imageIntensity, std::vector<std::vector<std::vector<float>>> _chroma)
{
  int res = 20;

  int sizeX = _chroma.size();
  int sizeY = _chroma[0].size();
  float regionSizeX = ceil(float(sizeX)/float(res));
  float regionSizeY = ceil(float(sizeY)/float(res));

  std::vector<std::vector<float>> prevDiffF0;


  m_totDiffF0.resize(m_iterations); // iterations
  for(int i =0; i<m_iterations; ++i)
  {
    m_totDiffF0[i].resize(sizeX);
    for(int j =0; j<sizeX; ++j)
    {
      m_totDiffF0[i][j].resize(sizeY);
    }
  }



  prevDiffF0.resize(sizeX);
  for(int i =0; i<sizeX; ++i)
  {
    prevDiffF0[i].resize(sizeY);
  }

  std::vector<std::vector<float>> albedoIntensityMap = _imageIntensity;

  // Tells us which region each pixel belongs to
  // First two vectors are the region x and y
  // the second two vectors are the pixels x and y
  std::vector<std::vector<std::vector<std::vector<int>>>> whichPixelWhichRegion;
  whichPixelWhichRegion.resize(int(regionSizeX));
  for(int i = 0; i<whichPixelWhichRegion.size(); ++i)
  {
    whichPixelWhichRegion[i].resize(int(regionSizeY));
  }

  // First two vectors are regions x and y
  // Third vector is the chroma region index
  // Fourth vector is the r g b
  std::vector<std::vector<std::vector<std::vector<float>>>> regions;
  regions.resize(int(regionSizeX));
  for(int i = 0; i<regions.size(); ++i)
  {
    regions[i].resize(int(regionSizeY));
  }

  // First two vectors are regions x and y
  std::vector<std::vector<float>> sum1;
  sum1.resize(int(regionSizeX));
  for(int i = 0; i<sum1.size(); ++i)
  {
    sum1[i].resize(int(regionSizeY));
    for(int j = 0; j<regionSizeY; ++j)
    {
      sum1[i][j]=0.0f;
    }
  }

  // First two vectors are regions x and y
  std::vector<std::vector<float>> sum2;
  sum2.resize(int(regionSizeX));
  for(int i = 0; i<sum2.size(); ++i)
  {
    sum2[i].resize(int(regionSizeY));
    for(int j = 0; j<regionSizeY; ++j)
    {
      sum2[i][j]=0.0f;
    }
  }

  // First two vectors are regions x and y
  // Third vector is the chroma region index
  std::vector<std::vector<std::vector<float>>> sum3;
  sum3.resize(int(regionSizeX));
  for(int i = 0; i<sum3.size(); ++i)
  {
    sum3[i].resize(int(regionSizeY));
  }

  // First two vectors are regions x and y
  // Third vector is the chroma region index
  std::vector<std::vector<std::vector<int>>> numberOfPixelsInChromaRegions;
  numberOfPixelsInChromaRegions.resize(int(regionSizeX));
  for(int i = 0; i<numberOfPixelsInChromaRegions.size(); ++i)
  {
    numberOfPixelsInChromaRegions[i].resize(int(regionSizeY));
  }
  float regionStep = 3.0f/20.0f;

  std::cout<<"HELLO\n";

  //------------------------FIND CHROMA REGIONS------------------------------------
  for(int x = 0; x<int(regionSizeX); ++x)
  {
    for(int y = 0; y<int(regionSizeY) ; ++y)
    {
      int indexY = res*y;
      if(indexY>_chroma[0].size()-1) break;

      int indexX = res*x;
      if(indexX>_chroma.size()-1) break;

      whichPixelWhichRegion[x][y].resize(res);
      // Now we go through the 20*20 region's pixels

      for(int i = 0; i<res; ++i)
      {
        whichPixelWhichRegion[x][y][i].resize(res);
        indexY = res*y;
        if(indexY>_chroma[0].size()-1) break;

        for(int j = 0; j<res; ++j)
        {

          // See if the pixel is in any existing regions.
          bool found = false;
          for(int z =0; z<regions[x][y].size(); ++z)
          {
            // If within an existing region
            if(_chroma[indexX][indexY][0]>regions[x][y][z][0] && _chroma[indexX][indexY][0]<regions[x][y][z][0]+regionStep &&
               _chroma[indexX][indexY][1]>regions[x][y][z][1] && _chroma[indexX][indexY][1]<regions[x][y][z][1]+regionStep)
            {
               numberOfPixelsInChromaRegions[x][y][z]++;
               whichPixelWhichRegion[x][y][i][j]=z;
               found = true;
               break;
            }
          }
          // If no existing chroma region
          if(!found)
          {
            numberOfPixelsInChromaRegions[x][y].push_back(1);
            whichPixelWhichRegion[x][y][i][j]=numberOfPixelsInChromaRegions[x][y].size()-1;
            // Quantize the chroma regions and add it to the list of regions
            float rTemp = _chroma[indexX][indexY][0]/regionStep;
            float gTemp = _chroma[indexX][indexY][0]/regionStep;
            float r = floor(rTemp);
            float g = floor(gTemp);
            std::vector<float> temp;
            temp.push_back(r);
            temp.push_back(g);
            regions[x][y].push_back(temp);
          }
          indexY++;
          if(indexY>=sizeY) break;
        }
        indexX++;
        if(indexX>=sizeX) break;
      }
    }
  }

  bool firstTime = true;

  for(int r =0; r<m_iterations; ++r)
  {
  //------------------------UPDATE SUMS------------------------------------

    // First two vectors are regions x and y
    for(int i = 0; i<sum1.size(); ++i)
    {
      for(int j = 0; j<sum1[i].size(); ++j)
      {
        sum1[i][j]=0.0f;
      }
    }

    // First two vectors are regions x and y
    for(int i = 0; i<sum2.size(); ++i)
    {
      for(int j = 0; j<sum2[i].size(); ++j)
      {
        sum2[i][j]=0.0f;
      }
    }

  for(int x = 0; x<int(regionSizeX); ++x)
  {
    for(int y = 0; y<int(regionSizeY) ; ++y)
    {
      int indexY = res*y;
      if(indexY>_chroma[0].size()-1) break;

      int indexX = res*x;
      if(indexX>_chroma.size()-1) break;

      sum3[x][y].resize(regions[x][y].size());
      for(int p = 0; p<sum3[x][y].size();++p)
      {
        sum3[x][y][p]=0.0f;
      }
      for(int i = 0; i<res; ++i)
      {
        indexY = res*y;
        if(indexY>_chroma[0].size()-1) break;

        for(int j = 0; j<res; ++j)
        {
          // CHeck if dividing by zero
          if(albedoIntensityMap[indexX][indexY]<0.000001f)
          {
            albedoIntensityMap[indexX][indexY] = 0.000001f;
          }

          sum1[x][y]+=_imageIntensity[indexX][indexY]/(albedoIntensityMap[indexX][indexY]*albedoIntensityMap[indexX][indexY]);
          sum2[x][y]+=_imageIntensity[indexX][indexY]/albedoIntensityMap[indexX][indexY];
          if(isinf(sum2[x][y])) std::cout<<"is inf: "<<albedoIntensityMap[indexX][indexY]<<"\n";
          sum3[x][y][whichPixelWhichRegion[x][y][i][j]]+=_imageIntensity[indexX][indexY];
          indexY++;
          if(indexY>=sizeY) break;
        }
        indexX++;
        if(indexX>=sizeX) break;
      }
    }
  }

  for(int x = 0; x<int(regionSizeX); ++x)
  {
    for(int y = 0; y<int(regionSizeY) ; ++y)
    {
      int indexY = res*y;
      if(indexY>_chroma[0].size()-1) break;

      int indexX = res*x;
      if(indexX>_chroma.size()-1) break;

      for(int i = 0; i<res; ++i)
      {
        indexY = res*y;
        if(indexY>_chroma[0].size()-1) break;
        for(int j = 0; j<res; ++j)
        {
          int ourChromaRegion = whichPixelWhichRegion[x][y][i][j];
          float noPixelsChroma = float(numberOfPixelsInChromaRegions[x][y][ourChromaRegion]);  

          float DiffF0 = 2*(1 + (1.0f/(20.0f*20.0f))*sum1[x][y] *
                            (((1.0f/noPixelsChroma)*sum3[x][y][ourChromaRegion])
                             /
                             (((1.0f/(20.0f*20.0f))*sum2[x][y])*((1.0f/(20.0f*20.0f))*sum2[x][y]))))
                            *
                            (albedoIntensityMap[indexX][indexY] - (((1.0f/noPixelsChroma)*sum3[x][y][noPixelsChroma])/((1.0f/(20.0f*20.0f))*sum2[x][y])));
          //std::cout<<"Time"<<r<<":"<<indexX<<", "<<indexY<<": "<<DiffF0<<"\n";
          m_totDiffF0[r][indexX][indexY] = DiffF0;
          //  Change albedo based on DiffF0
          if(DiffF0 > 0.1f) albedoIntensityMap[indexX][indexY]-=0.01f;
          else if(DiffF0 < -0.1f) albedoIntensityMap[indexX][indexY]+=0.01f;

          if(DiffF0 == 2.0f)
          {
//            std::cout<<"NAN ALERT\n";

//            std::cout << "sum1: " << sum1[x][y] << " sum2: " << sum2[x][y] << " sum3: " << sum3[x][y][ourChromaRegion] << "\n" ;
          }

          if(!firstTime)
          {
            if(abs(prevDiffF0[indexX][indexY]) > DiffF0)
            {
              std::cout<<"CONVERGING\n";
            }
            else
            {
              std::cout<<"DIVERGING\n";
            }
          }
          prevDiffF0[indexX][indexY] = DiffF0;

          indexY++;
          if(indexY>=sizeY) break;
        }
        indexX++;
        if(indexX>=sizeX) break;
      }
    }
  }

  firstTime=false;
  }
    exportCSV("result.csv");
 }


void GLWindow::exportCSV( std::string _file )
{
    std::ofstream out;
    out.open( _file );
    out.clear();
    for(int j =0; j< m_totDiffF0[0].size(); ++j)
    {
        for ( int k = 0; k <  m_totDiffF0[0][0].size(); ++k )
        {
            for(int i = 0; i< m_iterations; ++i)
            {
                out << m_totDiffF0[i][j][k] << ",";
            }
            out << "\n";
        }
    }

}
