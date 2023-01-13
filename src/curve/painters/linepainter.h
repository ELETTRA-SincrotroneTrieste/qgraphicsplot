#ifndef LINEPAINTER_H
#define LINEPAINTER_H

#include <itempainterinterface.h>
#include <QObject>
#include <QPen>

class LinePainterPrivate;

class LinePainter : public QObject, public ItemPainterInterface
{
    Q_PROPERTY(QColor lineColor READ lineColor WRITE setLineColor)
    Q_PROPERTY(double lineWidth READ lineWidth WRITE setLineWidth)
    Q_PROPERTY(QPen linePen READ linePen WRITE setLinePen)
    Q_OBJECT

public:
    LinePainter(CurveItem* curveItem);

    virtual ~LinePainter();

    virtual void draw(SceneCurve *curve,
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

    /** \brief Implements the pen method of the ItemPainterInterface
     *
     * @return the line pen as returned by linePen()
     */
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

private:
    Q_DECLARE_PRIVATE(LinePainter)

    LinePainterPrivate* d_ptr;
};

#endif // LINEPAINTER_H
