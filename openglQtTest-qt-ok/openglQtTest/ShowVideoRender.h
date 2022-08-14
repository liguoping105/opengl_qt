#ifndef SHOWVIDEORENDER_H
#define SHOWVIDEORENDER_H

#include <QOpenGLWidget>
//#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_0>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QWidget>
#include <QMutex>
#include <QMouseEvent>

#define TEXTURE_YUV 1
class ShowVideoRender : public QOpenGLWidget, protected QOpenGLFunctions_3_0
{
    Q_OBJECT
public:
    ShowVideoRender(QWidget *para);
    ~ShowVideoRender() override;

    void SetYUVData(unsigned char* dataY, unsigned char* dataU, unsigned char* dataV, int width, int height);

    void UpdataVideo();

    void SetIsShowFlag(bool isShow);
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    void updateYUVData();

 public:
  int                                 mVideoW, mVideoH;
private:
    bool isShow = false;

    QOpenGLShaderProgram               *pProgram;
    GLuint                              textureUniformY, textureUniformU, textureUniformV;

#if(1==TEXTURE_YUV)
    GLuint                              idY, idU, idV;

    QOpenGLTexture                     *pTextureY;
    QOpenGLTexture                     *pTextureU;
    QOpenGLTexture                     *pTextureV;
#endif
    QOpenGLShader                      *pVShader;
    QOpenGLShader                      *pFShader;



    unsigned char                      *pBufY;
    unsigned char                      *pBufU;
    unsigned char                      *pBufV;

    unsigned char                      *pBufY_Draw;
    unsigned char                      *pBufU_Draw;
    unsigned char                      *pBufV_Draw;

    size_t                              ySize;
    size_t                              u_vSize;

    QMutex                              mMutex;

};

#endif // SHOWVIDEORENDER_H
