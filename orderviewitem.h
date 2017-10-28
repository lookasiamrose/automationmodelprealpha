#ifndef ORDERVIEWITEM_H
#define ORDERVIEWITEM_H

#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsRectItem>
#include <QBrush>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QDataStream>
#include <QGraphicsProxyWidget>
#include <QComboBox>
#include "graphicsrectbuttonitem.h"

class OrderViewItem;

class OrderViewItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
    QGraphicsSimpleTextItem* name;
    QGraphicsSimpleTextItem valueLabel;

    QGraphicsLineItem* lineBarrier;
    QGraphicsLineItem* lineContinue;

    OrderViewItem* barrierOrder;
    OrderViewItem* continueOrder;
    OrderViewItem* inverseContinueOrder;
    OrderViewItem* inverseBarrierOrder;

    bool isGantryInner;

    QGraphicsRectItem* previous;
    QGraphicsLineItem* previousLine;
    QGraphicsRectItem* nextOrder;
    QGraphicsTextItem value;
    QComboBox* combo;
    QGraphicsProxyWidget* comboSibling;
    QComboBox* combo2;
    QGraphicsProxyWidget* comboSibling2;
    QGraphicsSimpleTextItem* type;
private:
    void createChildrenItems();
 public:
     OrderViewItem(bool isGantry = false, QGraphicsItem * parent = nullptr);
     OrderViewItem(int orderArgIndex, QString typeOfDeviceArg, bool isGantry = false, QGraphicsItem * parent = 0);
     ~OrderViewItem();
     QVariant itemChange(GraphicsItemChange change, const QVariant &value);
     void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
     void updateChildrenPositions( QPointF newPosArg);
     void linkLineBarrier(QGraphicsLineItem* lineArg);
     void linkLineContinue(QGraphicsLineItem* lineArg);
     void chainContinueOrder(OrderViewItem* nextOrderArg);
     void updateLines(bool breakTime = false);
     void chainBarrierOrder(OrderViewItem* nextOrderArg);
     void inverseChainContinueOrder(OrderViewItem* nextOrderArg);
     void inverseChainBarrierOrder(OrderViewItem* nextOrderArg);
     QLineF getContinueLineSchema();
     QLineF getBarrierLineSchema();
     void setPrevious(QGraphicsRectItem* prevArg);
     void setNextOrder(QGraphicsRectItem* nextArg);
     void updatePreviousLineItem();
     void getData(QDataStream* saveStream);
     void loadData(QDataStream* saveStream);
     QString prepareData();
     char convertComboMotorToChar();
     char getBarrierType();
     char getMyType();
     char convertComboGantryToChar();
     void setGantry();

     int orderIndex;
     QString typeOfDevice;

     int savedBarrierOrderIndex;
     int savedContinueOrderIndex;
     int savedInverseBarrierOrderIndex;
     int savedInverseContinueOrderIndex;
     GraphicsRectButtonItem* barrierButton;
     GraphicsRectButtonItem* continueButton;
};

#endif // ORDERVIEWITEM_H
