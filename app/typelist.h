#ifndef TYPELIST_H
#define TYPELIST_H


#define _USE_MATH_DEFINES
#define BLOCKRANGE 4096
#define BLOCKLANGTH 1024
#if !defined(MAXBYTE)
#define MAXBYTE 256
#endif
#define MAXBYTEFLOAT 256.0f
#define RAD 57.295779513082320877
#define MaxSpeed 50
#define QUINT16_SIZE 65535


#include <qopengl.h>
#include <QVector>
#include <QMap>
#include <cmath>




typedef struct sPlugs {
    bool enable;
    QString path;
    QString name;
    QString alias;
} Plugs;



typedef QVector<double> MathVector;
typedef QVector<GLfloat> Clowd;
typedef QVector<float> floatVector;
typedef QVector <unsigned int> IntVector;

typedef struct fData {
    int length;
    GLfloat *link;
} fdata_t;

const unsigned int AVG_DATA_LEN=1024*8*4;
const unsigned int MAX_PKT_DATA_LEN=1024;

const double Fd=2400000000.0f;
const double Td=(1.0/Fd);

const double Ti=10000*1e-6f;
const double Di=5*1.0e-6f;
const unsigned LenN=floor(Di/Td);

const double LFM_dev=300*1e6f;
const double LFM_step=(LFM_dev/LenN / 2);

const double Noise=0;



#endif // TYPELIST_H
