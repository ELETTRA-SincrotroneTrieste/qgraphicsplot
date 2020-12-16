#ifndef DOTSPAINTER_H
#define DOTSPAINTER_H

#include <itempainterinterface.h>
#include <QObject>
#include <QPen>

class DotsPainterPrivate;

class DotsPainter : public QObject, public ItemPainterInterface
{
    Q_PROPERTY(QColor dotsColor READ dotsColor WRITE setDotsColor)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
    Q_PROPERTY(double radius READ radius WRITE setRadius)
    Q_PROPERTY(QPen pen READ pen WRITE setPen)
    Q_OBJECT

public:
    DotsPainter(CurveItem* curveItem);

    virtual ~DotsPainter();

    virtual void draw(SceneCurve *curve,
                      PlotSceneWidget* plot,
                      QPainter *painter,
                      const QStyleOptionGraphicsItem * option,
                      QWidget * widget = 0);

    virtual CurveItem* curveItem() const;

    virtual QSizeF elementSize() const;

    virtual int type() const;

    QColor borderColor() const;

    /** \brief returns the colour used for the dots that form the curve
      *
      * @return the color of the dots
      *
      * @see setDotsColor
      * @see setRadius
      */
    QColor dotsColor() const;

    /** \brief returns the line width
      *
      * @return the width of the line used to draw the dots.
      *
      * @see setDotsColor
      * @see setRadius
      */
    double radius() const;

    /** \brief returns the pen used to draw the dots
      *
      * @return the pen used to draw the dots.
      */
    QPen pen() const;

public slots:

    /** \brief changes the color of the dots
      *
      * The default colour is yellow
      *
      * @see lineColor
      */
    void setDotsColor(const QColor& c);

    /** \brief changes the width of the dots
      *
      * The default is 0.0
      *
      * @see radius
      */
    void setRadius(double w);

    /** \brief changes the color of the dots
      *
      * @see linePen
      */
    void setPen(const QPen& p);

    /** \brief sets the border color of the dot.
      *
      */
    void setBorderColor(const QColor& c);


private:
    Q_DECLARE_PRIVATE(DotsPainter)

    DotsPainterPrivate* d_ptr;
};

#endif // LINEPAINTER_H
