#ifndef CONVERTER_H
#define CONVERTER_H

#include <QObject>
#include <QImage>

#include <opencv2/imgproc.hpp>

class converter
{
public:
    converter();

    static QImage Mat2QImage(cv::Mat const& src);
    static cv::Mat QImage2Mat(QImage const& src);
};

#endif // CONVERTER_H
