#ifndef STEPSPAINTER_H
#define STEPSPAINTER_H

#include <itempainterinterface.h>
#include <QObject>
#include <QPen>

class StepsPainterPrivate;

class StepsPainter : public QObject, public ItemPainterInterface
{
    Q_PROPERTY(QColor lineColor READ lineColor WRITE setLineColor)
    Q_PROPERTY(double lineWidth READ lineWidth WRITE setLineWidth)
    Q_PROPERTY(QPen linePen READ linePen WRITE setLinePen)

    Q_OBJECT

public:
    explicit StepsPainter(CurveItem *curveItem);

    virtual ~StepsPainter();

    virtual void draw(SceneCurve *curve,
                      PlotSceneWidget* plot,
                      QPainter *painter,
                      const QStyleOptionGraphicsItem * option,
                      QWidget * widget = 0);

    virtual CurveItem* curveItem() const;

    virtual QSizeF elementSize() const;

    virtual int type() const;

    /** \brief returns the colour used for the lines that form the curve
      *
      * @return the color of the lines
      *
      * @see setLineColor
      * @see setLineWidth
      */
    QColor lineColor() const;

    /** \brief returns the line width
      *
      * @return the width of the line used to draw the lines.
      *
      * @see setLineColor
      * @see setLineWidth
      */
    double lineWidth() const;

    /** \brief returns the pen used to draw the lines
      *
      * @return the pen used to draw the lines.
      */
    QPen linePen() const;

    QPen pen() const;

public slots:

    /** \brief changes the color of the lines
      *
      * The default colour is Qt::black
      *
      * @see lineColor
      */
    void setLineColor(const QColor& c);

    /** \brief changes the width of the lines
      *
      * The default is 0.0
      *
      * @see lineWidth
      */
    void setLineWidth(double w);

    /** \brief changes the color of the lines
      *
      * @see linePen
      */
    void setLinePen(const QPen& p);


signals:

public slots:

private:
    Q_DECLARE_PRIVATE(StepsPainter)

    StepsPainterPrivate* d_ptr;
};

#endif // STEPSPAINTER_H
