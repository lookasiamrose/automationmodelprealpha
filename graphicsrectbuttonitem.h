#ifndef GRAPHICSRECTBUTTONITEM_H
#define GRAPHICSRECTBUTTONITEM_H

#include <QGraphicsRectItem>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSimpleTextItem>

class OrderViewItem;

class GraphicsRectButtonItem : public QGraphicsRectItem
{
    OrderViewItem* father;
    QPointF initialPosition;
    QGraphicsSimpleTextItem name;
    bool isAttached;

public:
    GraphicsRectButtonItem(QString labelInsideArg, QPointF initialPosArg, QGraphicsItem * parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QVariant itemChange(GraphicsItemChange change, const QVariant &valueArg);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

#endif // GRAPHICSRECTBUTTONITEM_H
