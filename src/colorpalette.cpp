#include "colorpalette.h"


ColorPalette::ColorPalette()
{
    mColors << KDARKGREEN << EBLUEGRAY << KDARKMAROON << KDARKVIOLET << KBLUE
      << KMAROON << KRED << KVERYDARKVIOLET << KYELLOW << KORANGE << KDARKCYAN;
}

void ColorPalette::addColor(const QColor& c)
{
    mColors << c;
}

QColor ColorPalette::getColor(int index) const
{
    return mColors.at(index % mColors.size());
}
