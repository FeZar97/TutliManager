#ifndef CONVERTER_H
#define CONVERTER_H

#include <QObject>
#include <QImage>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "opencv/cv.h"
#include "opencv/ml.h"

class converter
{
public:
    converter();

    static IplImage *QImageToIplImage(const QImage * qImage);
    static QImage   *IplImageToQImage(const IplImage* iplImage, uchar** data,
                                      double mini=0.0, double maxi=0.0);
};

#endif // CONVERTER_H
