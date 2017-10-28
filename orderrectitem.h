#ifndef ORDERRECTITEM_H
#define ORDERRECTITEM_H

#include <QGraphicsRectItem>
#include <QObject>

class OrderRectItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    OrderRectItem(QGraphicsItem * parent = nullptr);
};

#endif // ORDERRECTITEM_H
