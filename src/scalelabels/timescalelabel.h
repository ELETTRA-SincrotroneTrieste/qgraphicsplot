#ifndef TIMESCALELABEL_H
#define TIMESCALELABEL_H

#include <scalelabelinterface.h>

class TimeScaleLabelPrivate
{
public:
    TimeScaleLabelPrivate() {}
    bool showDate;
};


class TimeScaleLabel : public ScaleLabelInterface
{
public:

    TimeScaleLabel(bool showDate = false);
    virtual ~TimeScaleLabel();
    virtual QString label(double value) const;

    void setShowDate(bool show);

    bool showDate() const;

    virtual ScaleLabelInterface::Type type() const;

    virtual QString longestLabel() const;


private:
    Q_DECLARE_PRIVATE(TimeScaleLabel)
    TimeScaleLabelPrivate *d_ptr;
};

#endif // TIMESCALELABEL_H
