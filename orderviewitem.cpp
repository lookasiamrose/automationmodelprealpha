#include "orderviewitem.h"
#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneDragDropEvent>


OrderViewItem::OrderViewItem(int orderArgIndex, QString typeOfDeviceArg, bool isGantry, QGraphicsItem * parent) : QObject(), QGraphicsRectItem(parent)
{
    this->setZValue(1);
    this->setBrush(Qt::white);

    isGantryInner = isGantry;

    type = nullptr;
    nextOrder = nullptr;
    previousLine = nullptr;
    previous = nullptr;
    lineBarrier = nullptr;
    lineBarrier = nullptr;
    continueOrder = nullptr;
    barrierOrder = nullptr;
    inverseContinueOrder = nullptr;
    inverseBarrierOrder = nullptr;

    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    QRectF r2(QPoint(100, 200), QSize(400, 55));
    setRect(r2);

    orderIndex = orderArgIndex;
    typeOfDevice = typeOfDeviceArg;
    createChildrenItems();
    name->setText("Order " + QString::number(orderIndex));

    setAcceptDrops(true);

    setData(1,"ORDER");


}
OrderViewItem::OrderViewItem(bool isGantry, QGraphicsItem * parent) : QObject(), QGraphicsRectItem(parent)
{
    this->setZValue(1);
    this->setBrush(Qt::white);

    isGantryInner = isGantry;
    type = nullptr;
    nextOrder = nullptr;
    previousLine = nullptr;
    previous = nullptr;
    lineBarrier = nullptr;
    lineBarrier = nullptr;
    continueOrder = nullptr;
    barrierOrder = nullptr;
    inverseContinueOrder = nullptr;
    inverseBarrierOrder = nullptr;

    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    QRectF r2(QPoint(100, 200), QSize(400, 55));
    setRect(r2);

    setAcceptDrops(true);

    setData(1,"ORDER");

    createChildrenItems();
}

void OrderViewItem::createChildrenItems()
{

    name = new QGraphicsSimpleTextItem(this);
    type = new QGraphicsSimpleTextItem(this);
    type->setText(typeOfDevice);
    value.setParentItem(this);
    valueLabel.setParentItem(this);
    name->setParentItem(this);

    QRectF basicRect = QRect( 0, 0, 25, 20 );
    QPointF initialBarrierPosition = QPointF(rect().x() + 5, rect().y() + 5);
    QPointF initialcontinuePosition = QPointF(rect().x() + 5, rect().y() + basicRect.height()  + 10);
    barrierButton = new GraphicsRectButtonItem("Barrier",initialBarrierPosition, this);
    continueButton = new GraphicsRectButtonItem("Continuous", initialcontinuePosition, this);

    barrierButton->setRect(basicRect);
    continueButton->setRect(basicRect);

    valueLabel.setText("Value:");
    value.setTextInteractionFlags(Qt::TextEditorInteraction);
    value.setPos(initialcontinuePosition);
    valueLabel.setPos(initialcontinuePosition);
    value.setTextWidth(100);
    value.setScale(2);

    /*QGroupBox* kappa = new QGroupBox();
    QFormLayout* layout = new QFormLayout;
    QLabel* optionLabel = new QLabel(" ");
    layout->addRow(optionLabel, &combo);
    combo.addItem("Clockwise");
    combo.addItem("Counterclockwise");

    kappa->setLayout(layout);
    */
    combo = new QComboBox();
    combo->addItem("Clockwise");
    combo->addItem("Counterclockwise");
    comboSibling = new QGraphicsProxyWidget(this);
    comboSibling->setWidget(combo);

    combo2 = new QComboBox();
    if(!isGantryInner)
    {
        combo2->addItem("Lean");
        combo2->addItem("Updown");
        combo2->addItem("Drive");
        combo2->addItem("Steering");
    }else{
        combo2->addItem("Turn");
        combo2->addItem("Move");
        combo2->addItem("Updown");
        combo2->addItem("LeftRight");
    }
    comboSibling2 = new QGraphicsProxyWidget(this);
    comboSibling2->setWidget(combo2);

    if(typeOfDevice != "Stepper motor order")
    {
        comboSibling->setVisible(false);
        comboSibling2->setVisible(false);
    }

    updateChildrenPositions( pos() );
}
QVariant OrderViewItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene())
    {
        updateChildrenPositions( value.toPointF() );
    }
    return QGraphicsItem::itemChange(change, value);
}
void OrderViewItem::updateChildrenPositions( QPointF newPosArg)
{
    int xCenterOffset = rect().x() + rect().width()/4;
    int yCenterOffset = rect().y() + rect().height()/2;

    QPointF newCenterPos = QPointF( xCenterOffset,  yCenterOffset);

    name->setPos( newCenterPos );

    newCenterPos = QPointF( xCenterOffset + rect().width()/4,  rect().y());

    value.setPos(newCenterPos);
    newCenterPos.setX(newCenterPos.x() - 50);
    newCenterPos.setY(rect().y() + 5);
    valueLabel.setPos(newCenterPos);

    comboSibling->setPos(rect().x()+rect().width()/2,rect().y()+rect().height());
    comboSibling2->setPos(rect().x() + 5,rect().y()+rect().height());
    type->setPos(rect().x()+5,rect().y()-20);

    updateLines();

    newPosArg = newPosArg; //nothing to no warning
}
QLineF OrderViewItem::getBarrierLineSchema()
{
    QLineF line;
    line.setLine(barrierOrder->scenePos().x()+barrierOrder->rect().x()
                 + barrierOrder->rect().width()/5,
            barrierOrder->scenePos().y()+barrierOrder->rect().y(),
            scenePos().x()+rect().x()+rect().width()/5,
            scenePos().y()+rect().y()+ barrierOrder->rect().height());
    return line;
}
QLineF OrderViewItem::getContinueLineSchema()
{
    QLineF line;
    line.setLine(continueOrder->scenePos().x()+continueOrder->rect().x()
                 +continueOrder->rect().width()*4/5,
           continueOrder->scenePos().y()+continueOrder->rect().y(),
           scenePos().x()+rect().x()+rect().width()*4/5,
           scenePos().y()+rect().y()+rect().height());
    return line;
}
void OrderViewItem::updateLines(bool breakTime)
{
    if(lineBarrier != nullptr && barrierOrder != nullptr)
    {
        lineBarrier->setLine( getBarrierLineSchema() );
        QList<QGraphicsItem*> names = lineBarrier->childItems();
        QGraphicsSimpleTextItem* nameTemp = dynamic_cast<QGraphicsSimpleTextItem*> (names.at(0));
        nameTemp->setPos(lineBarrier->boundingRect().x()+lineBarrier->boundingRect().width()/2,
                         lineBarrier->boundingRect().y()+lineBarrier->boundingRect().height()/2);
        QGraphicsPolygonItem* arrowTemp = dynamic_cast<QGraphicsPolygonItem*> (names.at(1));
        arrowTemp->setPos(lineBarrier->line().x1(),lineBarrier->line().y1());
        arrowTemp->setRotation(-lineBarrier->line().angle()-90);
    }
    if(lineContinue != nullptr && continueOrder != nullptr)
    {
        lineContinue->setLine( getContinueLineSchema() );
        QList<QGraphicsItem*> names = lineContinue->childItems();
        QGraphicsSimpleTextItem* nameTemp = dynamic_cast<QGraphicsSimpleTextItem*> (names.at(0));
        nameTemp->setPos(lineContinue->boundingRect().x()+lineContinue->boundingRect().width()/2,
                         lineContinue->boundingRect().y()+lineContinue->boundingRect().height()/2);
        QGraphicsPolygonItem* arrowTemp = dynamic_cast<QGraphicsPolygonItem*> (names.at(1));
        arrowTemp->setPos(lineContinue->line().x1(),lineContinue->line().y1());
        arrowTemp->setRotation(-lineContinue->line().angle()-90);
    }
    if(!breakTime)
    {
        if(inverseBarrierOrder != nullptr) inverseBarrierOrder->updateLines(true);
        if(inverseContinueOrder != nullptr) inverseContinueOrder->updateLines(true);
    }

    if(previous != nullptr)
    {
        QLineF previousLineSchema;
        previousLineSchema.setLine(previous->scenePos().x() + previous->rect().x() + previous->rect().width()/2,
               previous->scenePos().y()+previous->rect().y() + previous->rect().height()-5,
               scenePos().x()+rect().x()+rect().width()/2,
               scenePos().y()+rect().y()+5);
        QPen inPen;
        inPen.setWidth(2);

        if(previousLine == nullptr)
        {
            previousLine = this->scene()->addLine(previousLineSchema);
        }
        else
        {
            previousLine->setLine(previousLineSchema);
        }
        previousLine->setPen(inPen);
        previousLine->setZValue(-1);

        if(nextOrder != nullptr)
        {
            dynamic_cast<OrderViewItem*>(nextOrder)->updatePreviousLineItem();
        }
    }
}
void OrderViewItem::updatePreviousLineItem()
{
    QLineF previousLineSchema;
    previousLineSchema.setLine(previous->scenePos().x() + previous->rect().x() + previous->rect().width()/2,
           previous->scenePos().y()+previous->rect().y() + previous->rect().height()-5,
           scenePos().x()+rect().x()+rect().width()/2,
           scenePos().y()+rect().y()+5);
    if(previousLine != nullptr)
        previousLine->setLine(previousLineSchema);
}
void OrderViewItem::setNextOrder(QGraphicsRectItem* nextArg)
{
    nextOrder = nextArg;
}
OrderViewItem::~OrderViewItem()
{
    delete name;
    delete barrierButton;
    delete continueButton;
}
void OrderViewItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //created for the selection flag clear (will be lack of dotted rected over the object)
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;
    QGraphicsRectItem::paint(painter, &myOption, widget);
}
void OrderViewItem::linkLineBarrier(QGraphicsLineItem* lineArg)
{
    lineBarrier = lineArg;
}
void OrderViewItem::linkLineContinue(QGraphicsLineItem* lineArg)
{
    lineContinue = lineArg;
}
void OrderViewItem::chainContinueOrder(OrderViewItem* nextOrderArg)
{
    continueOrder = nextOrderArg;
}
void OrderViewItem::chainBarrierOrder(OrderViewItem* nextOrderArg)
{
    barrierOrder = nextOrderArg;
}
void OrderViewItem::inverseChainContinueOrder(OrderViewItem* nextOrderArg)
{
    inverseContinueOrder = nextOrderArg;
}
void OrderViewItem::inverseChainBarrierOrder(OrderViewItem* nextOrderArg)
{
    inverseBarrierOrder = nextOrderArg;
}
void OrderViewItem::setPrevious(QGraphicsRectItem* prevArg)
{
    previous = prevArg;
}
void OrderViewItem::getData(QDataStream* saveStream)
{
    QString valueOfOrder = value.toPlainText();
    QPointF pos = scenePos();

    int barrierOrderIndex = barrierOrder == nullptr ? -1 : barrierOrder->orderIndex;
    int continueOrderIndex = continueOrder == nullptr ? -1 : continueOrder->orderIndex;
    int inverseBarrierOrderIndex = inverseBarrierOrder == nullptr ? -1 : inverseBarrierOrder->orderIndex;
    int inverseContinueOrderIndex = inverseContinueOrder == nullptr ? -1 : inverseContinueOrder->orderIndex;

    *(saveStream)<<orderIndex<<typeOfDevice<<valueOfOrder<<pos.x()<<pos.y()
                <<barrierOrderIndex<<continueOrderIndex<<inverseBarrierOrderIndex<<inverseContinueOrderIndex
                <<combo->currentText()<<combo2->currentText();
}
void OrderViewItem::loadData(QDataStream* saveStream)
{
    QString valueToLoad;
    int indexToLoad;
    QString typeOfDeviceToLoad;
    QString combo1Text, combo2Text;

    qreal x;
    qreal y;

    int barrierOrderIndex;
    int continueOrderIndex;
    int inverseBarrierOrderIndex;
    int inverseContinueOrderIndex;

    *(saveStream)>>indexToLoad>>typeOfDeviceToLoad>>valueToLoad>>x>>y
            >>barrierOrderIndex>>continueOrderIndex>>inverseBarrierOrderIndex>>inverseContinueOrderIndex
            >>combo1Text>>combo2Text;

    setPos(x, y);
    value.setPlainText(valueToLoad);
    typeOfDevice = typeOfDeviceToLoad;
    orderIndex = indexToLoad;

    if(typeOfDevice != "Stepper motor order")
    {
        comboSibling->setVisible(false);
        comboSibling2->setVisible(false);
    }
    else
    {
        comboSibling->setVisible(true);
        comboSibling2->setVisible(true);
    }

    if(type == nullptr)
        type = new QGraphicsSimpleTextItem(this);
    type->setText(typeOfDevice);

    name->setText("Order " + QString::number(orderIndex));

    combo->setCurrentText(combo1Text);
    combo2->setCurrentText(combo2Text);

    savedBarrierOrderIndex = barrierOrderIndex;
    savedContinueOrderIndex = continueOrderIndex;
    savedInverseBarrierOrderIndex = inverseBarrierOrderIndex;
    savedInverseContinueOrderIndex = inverseContinueOrderIndex;
}
void OrderViewItem::setGantry()
{
    isGantryInner = true;
}

QString OrderViewItem::prepareData()
{
    QString finalString;
    quint16 number = value.toPlainText().toInt(); //lets take the 16 bits representation of string
    quint16 numberBackup = number;
    quint8  number_a = 0xff & number; //clear first 8 bits with mask to be sure
    quint8  number_b = 0xff & (number>>8); //clear second 8 bits with mask to be sure
    char a = number_a; //char representation
    char b = number_b;

    qDebug()<<number_a;
    qDebug()<<number_b;

    if(numberBackup < 256) b = (char) 0x00;

    if(!isGantryInner){
        if( typeOfDevice  != "Servo motor order"){
            finalString.append(convertComboMotorToChar());
        }else{
            finalString.append('s');
        }
    }else{
        if( typeOfDevice  != "Servo motor order" ){
            finalString.append(convertComboGantryToChar());
        }else{
            finalString.append('s');
        }
    }
    char second = combo->currentText() == "Clockwise" ? '>' : '<';
    finalString.append(second);
    //value:
    finalString.append(a);
    finalString.append(b);

    quint8  barrier_8bit = barrierOrder != nullptr ? (barrierOrder->orderIndex == -1
                           ? 0 : barrierOrder->orderIndex) : 0;
    if(typeOfDevice  == "Servo motor order")
    {
        barrier_8bit += 128;
    }
    qDebug()<<"index - "<<orderIndex;
    finalString.append((char) barrier_8bit); //barrier

    quint8  index_8bit = orderIndex;
    finalString.append((char) index_8bit); //index

    if(!isGantryInner)
    {
        qDebug()<<" barrier - "<<(barrierOrder != nullptr ? barrier_8bit : 0);
        finalString.append(barrierOrder != nullptr ? getBarrierType() : 0);
    }else
    {
        finalString.append(barrierOrder != nullptr ? getMyType() : 0);
    }

    return finalString;
}

char OrderViewItem::convertComboMotorToChar()
{
    if(combo2->currentText() == "Lean")
    {
        return 'l';
    }else if(combo2->currentText() == "Updown")
    {
        return 'u';
    }else if(combo2->currentText() == "Drive")
    {
        return 'd';
    }else if(combo2->currentText() == "Steering")
    {
        return 's';
    }
    return '0';
}
char OrderViewItem::convertComboGantryToChar()
{
    if(combo2->currentText() == "Move")
    {
        return 'm';
    }else if(combo2->currentText() == "Updown")
    {
        return 'u';
    }else if(combo2->currentText() == "Turn")
    {
        return 't';
    }else if(combo2->currentText() == "LeftRight")
    {
        return 'l';
    }
    return '0';
}
char OrderViewItem::getBarrierType()
{
    if(barrierOrder->typeOfDevice == "Stepper motor order")
    {
        return 's';
    }else if(barrierOrder->typeOfDevice  == "Sensors barrier order")
    {
        return 'b';
    }else if(barrierOrder->typeOfDevice  == "Servo motor order")
    {
        return 'o';
    }
    return '0';
}
char OrderViewItem::getMyType()
{
    if(barrierOrder->typeOfDevice == "Stepper motor order")
    {
        return 'n';
    }else if(barrierOrder->typeOfDevice  == "Sensors barrier order")
    {
        return 'n';
    }else if(barrierOrder->typeOfDevice  == "Servo motor order")
    {
        return 'y';
    }
    return '0';
}
