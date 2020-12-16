#include "verticalscalewidget.h"

VerticalScaleWidget::VerticalScaleWidget(QWidget* parent)
    : ExternalScaleWidget(parent, ScaleItem::Vertical)
{
    /* distantiate labels from ticks in the vertical scale.
     * It helps when dealing with negative numbers.
     */
    setLabelDistFromTick(8);
}
