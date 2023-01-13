#ifndef HISTOGRAMPAINTER_H
#define HISTOGRAMPAINTER_H

class CurveItem;
class HistogramPainterPrivate;

#include <QObject>
#include <itempainterinterface.h>
#include <QColor>

class HistogramPainter :  public QObject, public ItemPainterInterface
{
    Q_PROPERTY(double width READ width WRITE setWidth)
    Q_PROPERTY(double baseLine READ baseLine WRITE setBaseLine)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(bool autoWidth READ autoWidth WRITE setAutoWidth)

    Q_OBJECT

public:
    HistogramPainter(CurveItem* curveItem);

    virtual void draw(SceneCurve *curve,
                      QPainter *painter,
                      const QStyleOptionGraphicsItem * option,
                      QWidget * widget = 0);

    virtual CurveItem* curveItem() const;

    virtual QSizeF elementSize() const;

    virtual int type() const;

    QColor color() const;

    double width() const;

    double baseLine() const;

    bool autoWidth() const;

    QColor colorAt(int index) const;

    QPen pen() const;

public slots:

    void setAutoWidth(bool aw);

    void setWidth(double w);

    void setBaseLine(double baseline);

    void setColor(const QColor& c);

    void setColorAt(int index, const QColor& c);

private:

    Q_DECLARE_PRIVATE(HistogramPainter)

    HistogramPainterPrivate *d_ptr;
};

#endif // HISTOGRAMPAINTER_H
