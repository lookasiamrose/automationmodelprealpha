#include "graphicsrectbuttonitem.h"
#include "orderviewitem.h"
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsScene>
#include <QApplication>
#include <QMimeData>
#include <QDebug>
#include <QDrag>

GraphicsRectButtonItem::GraphicsRectButtonItem(QString labelInsideArg, QPointF initialPosArg, QGraphicsItem * parent) : QGraphicsRectItem(parent)
{
    initialPosition = initialPosArg;
    isAttached = false;
    father = dynamic_cast<OrderViewItem*> (parent);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges); 
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);

    name.setText(labelInsideArg);
    name.setParentItem(this);
    setPos(initialPosition);
    name.setPos(0,0);
}
void GraphicsRectButtonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //created for the selection flag clear (will be lack of dotted rected over the object)
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;
    QGraphicsRectItem::paint(painter, &myOption, widget);
}
QVariant GraphicsRectButtonItem::itemChange(GraphicsItemChange change, const QVariant &valueArg)
{
    if (change == ItemPositionChange && scene())
    {
        QPointF actualPos = valueArg.toPointF();
        actualPos.setX(actualPos.x() + 5);
        actualPos.setY(actualPos.y() + 5);
        name.setPos(0,0);
    }
    return QGraphicsItem::itemChange(change, valueArg);
}
void GraphicsRectButtonItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);

    QList<QGraphicsItem*> coll = this->collidingItems();
    if(!coll.isEmpty())
    {
        foreach (QGraphicsItem* item, coll) {
            if(father != item)
            {
                if( item->data(1) == "ORDER")
                {

                    QPolygonF polRaw;
                    polRaw.append(QPointF(0,0));
                    polRaw.append(QPointF(-12.5,1.732*12.5));
                    polRaw.append(QPointF(12.5,1.732*12.5));

                    OrderViewItem* tempCast = dynamic_cast<OrderViewItem*> (item);
                    OrderViewItem* fatherCast = dynamic_cast<OrderViewItem*> (father);

                    if(name.text() == "Barrier")
                    {
                        fatherCast->chainBarrierOrder(tempCast);
                        tempCast->inverseChainBarrierOrder(fatherCast);

                        QGraphicsPolygonItem* poli = father->scene()->addPolygon(polRaw);

                        QGraphicsLineItem* line = father->scene()->addLine( father->getBarrierLineSchema() );
                        fatherCast->linkLineBarrier(line);

                        QGraphicsSimpleTextItem* n = father->scene()->addSimpleText("Barrier");
                        n->setParentItem(line);
                        n->setZValue(2);
                        line->setZValue(-1);
                        //QPen random;
                        //random.setWidth(2);
                        //line->setPen(random);

                        poli->setParentItem(line);
                        poli->setZValue(3);
                        poli->setBrush(Qt::white);

                    }else if(name.text() == "Continuous")
                    {
                        fatherCast->chainContinueOrder(tempCast);
                        tempCast->inverseChainContinueOrder(fatherCast);

                        QGraphicsPolygonItem* poli = father->scene()->addPolygon(polRaw);

                        QGraphicsLineItem* line = father->scene()->addLine( father->getContinueLineSchema() );
                        fatherCast->linkLineContinue(line);

                        QGraphicsSimpleTextItem* n = father->scene()->addSimpleText("Continuous");
                        n->setParentItem(line);
                        poli->setParentItem(line);
                        n->setZValue(2);
                        line->setZValue(-1);
                        poli->setZValue(3);
                        poli->setBrush(Qt::white);
                    }

                    fatherCast->updateLines();

                    setVisible(false);

                    break;
                }
            }else
            {
                setPos(initialPosition);
            }
        }
    }else
    {
        setPos(initialPosition);
    }
}
