#ifndef ITEMPOSITIONCHANGELISTENER_H
#define ITEMPOSITIONCHANGELISTENER_H

class QPointF;

class ItemPositionChangeListener
{
public:
    ItemPositionChangeListener();

    virtual void onItemPositionChanged(const QPointF& newPos) = 0;
};

#endif // ITEMPOSITIONCHANGELISTENER_H
