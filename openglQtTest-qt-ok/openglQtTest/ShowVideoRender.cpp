#include <QDebug>

#define CHECK_CALLOC(ptr, msg) if(ptr == nullptr){ \
                                   qDebug() << __FILE__ << "  " << __FUNCTION__ << "  " << __LINE__ << "  " << msg; \
                                   return; \
                               }

#include "ShowVideoRender.h"

ShowVideoRender::ShowVideoRender(QWidget *para) : QOpenGLWidget(para)
{
    this->textureUniformY = 0;
    this->textureUniformU = 0;
    this->textureUniformV = 0;

#if(1==TEXTURE_YUV)
    this->idY = 0;
    this->idU = 0;
    this->idV = 0;
#endif
    this->pVShader = nullptr;
    this->pFShader = nullptr;

    this->pProgram = nullptr;

#if(1==TEXTURE_YUV)
    this->pTextureY = nullptr;
    this->pTextureU = nullptr;
    this->pTextureV = nullptr;
#endif
    this->pBufY = nullptr;
    this->pBufU = nullptr;
    this->pBufV = nullptr;

    this->pBufY_Draw = nullptr;
    this->pBufU_Draw = nullptr;
    this->pBufV_Draw = nullptr;

    this->mVideoW = 0;
    this->mVideoH = 0;

    this->ySize = 0;
    this->u_vSize = 0;
}

ShowVideoRender::~ShowVideoRender()
{
    this->textureUniformY = 0;
    this->textureUniformU = 0;
    this->textureUniformV = 0;

#if(1==TEXTURE_YUV)
    this->idY = 0;
    this->idU = 0;
    this->idV = 0;

    if(this->pTextureY){ delete pTextureY; this->pTextureY = nullptr;}
    if(this->pTextureU){ delete pTextureU; this->pTextureU = nullptr;}
    if(this->pTextureV){ delete pTextureV; this->pTextureV = nullptr;}
#endif
    if(this->pVShader){ delete pVShader; this->pVShader = nullptr;}

    if(this->pFShader){ delete pFShader; this->pFShader = nullptr;}

    if(this->pProgram){ delete pProgram; this->pProgram = nullptr;}

    if(this->pBufY) { free(pBufY); this->pBufY = nullptr;}
    if(this->pBufU) { free(pBufU); this->pBufU = nullptr;}
    if(this->pBufV) { free(pBufV); this->pBufV = nullptr;}

    if(this->pBufY_Draw) { free(pBufY_Draw); this->pBufY_Draw = nullptr;}
    if(this->pBufU_Draw) { free(pBufU_Draw); this->pBufU_Draw = nullptr;}
    if(this->pBufV_Draw) { free(pBufV_Draw); this->pBufV_Draw = nullptr;}

    this->mVideoW = 0;
    this->mVideoH = 0;
    this->ySize = 0;
    this->u_vSize = 0;
}

void ShowVideoRender::SetYUVData(unsigned char *dataY, unsigned char *dataU, unsigned char *dataV, int width, int height)
{

    if(width == 0 || height == 0){
        return;
    }
    mMutex.lock();
    if(this->mVideoW != width || this->mVideoH != height){
        if(this->pBufY) { free(pBufY); this->pBufY = nullptr;}
        if(this->pBufU) { free(pBufU); this->pBufU = nullptr;}
        if(this->pBufV) { free(pBufV); this->pBufV = nullptr;}

        if(this->pBufY_Draw) { free(pBufY_Draw); this->pBufY_Draw = nullptr;}
        if(this->pBufU_Draw) { free(pBufU_Draw); this->pBufU_Draw = nullptr;}
        if(this->pBufV_Draw) { free(pBufV_Draw); this->pBufV_Draw = nullptr;}

        this->mVideoW = width;
        this->mVideoH = height;
        this->ySize = this->mVideoW * this->mVideoH;
        this->u_vSize = this->mVideoW * this->mVideoH / 4;

        this->pBufY = (unsigned char*)calloc(1, sizeof(unsigned char) * this->ySize);
        CHECK_CALLOC(this->pBufY, "calloc pBufY error.");
        this->pBufU = (unsigned char*)calloc(1, sizeof(unsigned char) * this->u_vSize);
        CHECK_CALLOC(this->pBufU, "calloc pBufU error.");
        this->pBufV = (unsigned char*)calloc(1, sizeof(unsigned char) * this->u_vSize);
        CHECK_CALLOC(this->pBufV, "calloc pBufV error.");

        this->pBufY_Draw = (unsigned char*)calloc(1, sizeof(unsigned char) * this->ySize);
        CHECK_CALLOC(this->pBufY_Draw, "calloc pBufY_Draw error.");
        this->pBufU_Draw = (unsigned char*)calloc(1, sizeof(unsigned char) * this->u_vSize);
        CHECK_CALLOC(this->pBufU_Draw, "calloc pBufU_Draw error.");
        this->pBufV_Draw = (unsigned char*)calloc(1, sizeof(unsigned char) * this->u_vSize);
        CHECK_CALLOC(this->pBufV_Draw, "calloc pBufV_Draw error.");
    }
    if(dataY && this->pBufY){
        memcpy(this->pBufY, dataY, this->ySize);
    }
    if(dataU && this->pBufU){
        memcpy(this->pBufU, dataU, this->u_vSize);
    }
    if(dataV && this->pBufV){
        memcpy(this->pBufV, dataV, this->u_vSize);
    }
    mMutex.unlock();
}

void ShowVideoRender::UpdataVideo()
{
    update();
}

void ShowVideoRender::SetIsShowFlag(bool isShow)
{
    this->isShow = isShow;
}

void ShowVideoRender::initializeGL()
{
    initializeOpenGLFunctions();
        static const char *vertexShaderSource1 =
                "attribute  vec3 posAttr;\n"
                "attribute  vec3 colAttr;\n"
                "varying  vec3 col;\n"
                "uniform  mat4 matrix;\n"
                "void main() {\n"
                "   col = colAttr;\n"
                "   gl_Position = matrix * vec4(posAttr, 1.0);\n"
                "}\n";
        static const char *fragmentShaderSource1 =
                "varying  vec3 col;\n"
                "void main() {\n"
                "   gl_FragColor = vec4(1.f, 0.f, 0.f, 1.f);\n"
                "}\n";

       



    
    static const char *vertexShaderSource =
               "attribute  vec3 posAttr;\n"
               "attribute  vec3 colAttr;\n"
               "varying  vec3 col;\n"
               "uniform  mat4 matrix;\n"
               "void main() {\n"
               "   col = colAttr;\n"
               "   gl_Position = matrix * vec4(posAttr, 1.0);\n"
               "}\n";
       static const char *fragmentShaderSource =
               "varying  vec3 col;\n"
               "void main() {\n"
               "   gl_FragColor = vec4(1.f, 0.f, 0.f, 1.f);\n"
               "}\n";
    qDebug()<<"ShowVideoRender::initializeGL()------1111";
    

    const char *vsrc = "attribute vec4 vertexIn;                                                         \
                        attribute vec2 textureIn;                                                        \
                        varying vec2 textureOut;                                                         \
                        void main(void){                                                                 \
                            gl_Position = vertexIn;                                                      \
                            textureOut = textureIn;                                                      \
                        }";

    const char *fsrc = "varying vec2 textureOut;                                                         \
                        uniform sampler2D tex_y;                                                         \
                        uniform sampler2D tex_u;                                                         \
                        uniform sampler2D tex_v;                                                         \
                        void main(void){                                                                 \
                            vec3 yuv;                                                                    \
                            vec3 rgb;                                                                    \
                            yuv.x = texture2D(tex_y, textureOut).r;                                      \
                            yuv.y = texture2D(tex_u, textureOut).r - 0.5;                                \
                            yuv.z = texture2D(tex_v, textureOut).r - 0.5;                                \
                            rgb = mat3(1,                1,                1,                            \
                                       0,               -0.39465,          2.03211,                      \
                                       1.13983,         -0.58060,          0        ) * yuv;             \
                            gl_FragColor = vec4(rgb, 1);                                                 \
                        }";

    this->pVShader = new QOpenGLShader(QOpenGLShader::Vertex  , this);
    this->pFShader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    bool bCompile = this->pVShader->compileSourceCode(vsrc);
    if(!bCompile){
        CHECK_CALLOC(nullptr, "compileSourceCode vsrc error.");
        qDebug() << vsrc;
        return;
    }

    bCompile = this->pFShader->compileSourceCode(fsrc);
    if(!bCompile){
        CHECK_CALLOC(nullptr, "compileSourceCode fsrc error.");
        qDebug() << fsrc;
        return;
    }

    this->pProgram = new QOpenGLShaderProgram();
    this->pProgram->addShader(this->pVShader);
    this->pProgram->addShader(this->pFShader);

    this->pProgram->bindAttributeLocation("vertexIn", 0);
    this->pProgram->bindAttributeLocation("textureIn", 1);
    this->pProgram->link();
    this->pProgram->bind();

    this->pProgram->enableAttributeArray(0);
    this->pProgram->enableAttributeArray(1);

    this->pProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
    this->pProgram->setAttributeBuffer(1, GL_FLOAT, 8 * sizeof(GLfloat), 2, 2 * sizeof(GLfloat));

    this->textureUniformY = this->pProgram->uniformLocation("tex_y");
    this->textureUniformU = this->pProgram->uniformLocation("tex_u");
    this->textureUniformV = this->pProgram->uniformLocation("tex_v");

    static const GLfloat vertexVertices[] =
    {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
    };

    static const GLfloat textureVertices[] =
    {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
    };

    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, vertexVertices);
    glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, textureVertices);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

#if(1==TEXTURE_YUV)
    this->pTextureY = new QOpenGLTexture(QOpenGLTexture::Target2D);
    this->pTextureU = new QOpenGLTexture(QOpenGLTexture::Target2D);
    this->pTextureV = new QOpenGLTexture(QOpenGLTexture::Target2D);

    this->pTextureY->create();
    this->pTextureU->create();
    this->pTextureV->create();

    this->idY = pTextureY->textureId();
    this->idU = pTextureU->textureId();
    this->idV = pTextureV->textureId();
#endif
    glClearColor(0.0, 0.0, 0.0, 0.0);
}

void ShowVideoRender::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void ShowVideoRender::paintGL()
{


    if(isShow){

        updateYUVData();

        //glEnable(GL_DEPTH_TEST);
        pProgram->bind();

        if(this->pBufY_Draw){
            glActiveTexture(GL_TEXTURE0);
#if(1==TEXTURE_YUV)
        glBindTexture(GL_TEXTURE_2D, idY);
#endif
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mVideoW, mVideoH, 0, GL_RED, GL_UNSIGNED_BYTE, pBufY_Draw);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    if(this->pBufU_Draw){
        glActiveTexture(GL_TEXTURE1);
#if(1==TEXTURE_YUV)
        glBindTexture(GL_TEXTURE_2D, idU);
#endif
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mVideoW / 2, mVideoH / 2, 0, GL_RED, GL_UNSIGNED_BYTE, pBufU_Draw);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    if(this->pBufV_Draw){
        glActiveTexture(GL_TEXTURE2);
#if(1==TEXTURE_YUV)
        glBindTexture(GL_TEXTURE_2D, idV);
#endif
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mVideoW / 2, mVideoH / 2, 0, GL_RED, GL_UNSIGNED_BYTE, pBufV_Draw);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    glUniform1i(textureUniformY, 0);
    glUniform1i(textureUniformU, 1);
    glUniform1i(textureUniformV, 2);

    static const GLfloat vertexVertices[] =
    {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
    };

    static const GLfloat textureVertices[] =
    {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
    };

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, vertexVertices);
    glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, textureVertices);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);


    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);//GL_TRIANGLE_FAN   GL_TRIANGLES
    glUseProgram(0);  pProgram->release();
  }
}

void ShowVideoRender::updateYUVData()
{
    mMutex.lock();
    if(this->pBufY && this->pBufY_Draw){
        memcpy(this->pBufY_Draw, this->pBufY, this->ySize);
    }
    if(this->pBufU && this->pBufU_Draw){
        memcpy(this->pBufU_Draw, this->pBufU, this->u_vSize);
    }
    if(this->pBufV && this->pBufV_Draw){
        memcpy(this->pBufV_Draw, this->pBufV, this->u_vSize);
    }
    mMutex.unlock();
}
