#ifndef ITEMMOVELISTENER_H
#define ITEMMOVELISTENER_H

class QPointF;

class ItemMoveListener
{
public:
    ItemMoveListener();

    virtual void itemMoved(const QPointF& newPos) = 0;
};

#endif // ITEMMOVELISTENER_H
