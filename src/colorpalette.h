#ifndef COLORPALETTE_H
#define COLORPALETTE_H

#include <QList>
#include <QColor>
#include "colors.h"

class ColorPalette
{
public:
    ColorPalette();

    QColor getColor(int index) const;

    void addColor(const QColor& c);

private:

    QList<QColor> mColors;
};

#endif // COLORPALETTE_H
