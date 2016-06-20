#include "typelist.h"
#include "memory.h"
#include "nomain/rdata.h"
#include "gwindow.h"
#include <QApplication>
#include <QSurfaceFormat>
#include <QOpenGLContext>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
        format.setVersion(3, 3);
        format.setProfile(QSurfaceFormat::CompatibilityProfile);
    }else {
        format.setVersion(3, 0);
    }
/*        format.setVersion(4, 2);
        format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setRenderableType(QSurfaceFormat::OpenGLES);*/
//        format.setStencilBufferSize(32);
        QSurfaceFormat::setDefaultFormat(format);

    qRegisterMetaType<Clowd>("Clowd");
    qRegisterMetaType<MathVector>("MathVector");
    qRegisterMetaType<IntVector>("IntVector");
    qRegisterMetaType<IntVector>("IntVector&");
    qRegisterMetaType<QByteArray>("QByteArray&");
    qRegisterMetaType<Clowd>("Clowd&");



    QCoreApplication::setOrganizationName("Nixson LLC");
    QCoreApplication::setOrganizationDomain("nixson.ru");
    QCoreApplication::setApplicationName("Rip3P");

//    Memory::dLink = new Rdata();
    GWindow w;
    w.show();

    return a.exec();
}
