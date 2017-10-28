#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QActionGroup>
#include <QDebug>
#include <QGraphicsRectItem>
#include <QFile>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    uart = new Uart();
    QList<qint32> bauds = uart->getStandardBaudRates();
    if(!bauds.isEmpty())
        uart->setBaudRate(bauds.at(0));

    newAct = new QAction(tr("&Ports information"), this);
    newAct->setStatusTip(tr("Show connected COM ports"));
    connect(newAct, &QAction::triggered, this, &MainWindow::com_ports);
    fileMenu = menuBar()->addMenu(tr("&Settings"));
    saveMenu = menuBar()->addMenu(tr("&Project"));
    fileMenu->addAction(newAct);

    QAction* saveDataToFile = new QAction(tr("&Save to file"), this);
    saveMenu->addAction(saveDataToFile);
    connect(saveDataToFile, &QAction::triggered, this, &MainWindow::saveData);

    QAction* loadDataFromFile = new QAction(tr("&Load from file"), this);
    saveMenu->addAction(loadDataFromFile);
    connect(loadDataFromFile, &QAction::triggered, this, &MainWindow::loadData);

    subMenu = new QMenu("Ports");
    baudMenu = new QMenu("Baud rates");
    fileMenu->addMenu(subMenu);
    fileMenu->addMenu(baudMenu);

    QList<QString> coms = uart->allCOMPorts();
    foreach(QString port , coms){
        actionPorts.append(new QAction(port));
    }

    QActionGroup* portGroup = new QActionGroup(this);
    foreach (QAction* action, actionPorts) {
        portGroup->addAction(action);
        action->setCheckable(true);
        subMenu->addAction(action);
    }

    if(actionPorts.isEmpty()){
        QAction* emptyAction = new QAction("No ports available");
        subMenu->addAction(emptyAction);
    }

    QActionGroup* baudGroup = new QActionGroup(this);
    foreach(qint32 one , bauds)
    {
        QString stringBaud;
        stringBaud = QString::number(one);
        QAction *singleBaud = new QAction(stringBaud);
        baudGroup->addAction(singleBaud);
        singleBaud->setCheckable(true);
        baudMenu->addAction(singleBaud);
    }

    connect(subMenu, SIGNAL(triggered(QAction*)), this, SLOT(menuSelection(QAction*)));
    connect(baudMenu, SIGNAL(triggered(QAction*)), this, SLOT(baudSelection(QAction*)));
    connect(ui->listWidget,SIGNAL(rightClick(QPoint*)),this,SLOT(onRightClick(QPoint*)));

    ui->listWidget->setDragDropMode(QAbstractItemView::InternalMove);

    ui->label_2->setStyleSheet("QLabel { background-color : darkMagenta; color : white; }");

    ordersCreationScene = new QGraphicsScene(this);
    ui->ordersCreationView->setScene(ordersCreationScene);

    ui->comboBox->addItem("Add Order");
    ui->comboBox->addItem("Stepper motor order");
    ui->comboBox->addItem("Sensors barrier order");
    ui->comboBox->addItem("Servo motor order");

    ui->comboBox_2->addItem("Choose device");
    ui->comboBox_2->addItem("Forklift");
    ui->comboBox_2->addItem("Gantry");

    prepareFirstAppearance();
    forkliftOrders = 1;
    gantryOrders = 1;
}
void MainWindow::prepareFirstAppearance()
{
    QGraphicsRectItem* device;
    QGraphicsSimpleTextItem* name;
    int xCenterOffset, yCenterOffset;
    QPointF newCenterPos;

    device = ordersCreationScene->addRect(-300,0,100,50);
    device->setBrush(Qt::white);
    name = ordersCreationScene->addSimpleText("Forklift");
    device->setData(0, "Forklift");
    xCenterOffset = device->rect().x() + device->rect().width()/4;
    yCenterOffset = device->rect().y() + device->rect().height()/4;
    newCenterPos = QPointF( xCenterOffset,  yCenterOffset);
    name->setPos( newCenterPos );
    name->setParentItem(device);
    forkliftStart = device;

    device = ordersCreationScene->addRect(200,0,100,50);
    device->setBrush(Qt::white);
    name = ordersCreationScene->addSimpleText("Gantry");
    device->setData(0, "Gantry");
    xCenterOffset = device->rect().x()  + device->rect().width()/4;
    yCenterOffset = device->rect().y() + device->rect().height()/4;
    newCenterPos = QPointF( xCenterOffset,  yCenterOffset);
    name->setPos( newCenterPos );
    name->setParentItem(device);
    gantryStart = device;
}
void MainWindow::baudSelection(QAction* baud)
{
    uart->setBaudRate(baud->text().toInt());
    if(!uart->getPortName().isEmpty()) uart->reopenPort();
}
void MainWindow::menuSelection(QAction* item)
{
    uart->setPortName(item->text());
    uart->reopenPort();
}
void MainWindow::com_ports()
{
    QList<QString> list = uart->checkAvailablePorts();
    foreach(QString port , list){
        QMessageBox::information(this,"Ports",port,QMessageBox::Ok);
    }
    if(list.isEmpty()){
        QMessageBox::warning(this,"Ports","No COM ports detected");
    }
}
MainWindow::~MainWindow()
{
    foreach (OrderViewItem* orderI, orderItemsGantry) {
        delete orderI;
    }
    foreach (OrderViewItem* orderI, orderItemsForklift) {
        delete orderI;
    }

    delete uart;
    delete ui;
}
void MainWindow::on_pushButton_clicked()
{
    QListWidgetItem* item = new QListWidgetItem(ui->lineEdit->text(), ui->listWidget);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Checked);
    ui->lineEdit->clear();
}
void MainWindow::on_pushButton_2_clicked()
{
    char c = ' ';
    for(int i=0;i<ui->listWidget->count();i++){
        QListWidgetItem* item = ui->listWidget->item(i);
        if( item->checkState() ==  Qt::Checked) {
            if(item->backgroundColor() == Qt::darkMagenta)
            {
                uart->writeNumberTwoByteFromString(item->text());
            }
            else
                {
                    if( (!item->text().isEmpty()) && (!item->text().isNull()) ) {
                        for(int pos=0;pos<item->text().size();pos++)
                        {
                            c = item->text().at(pos).toLatin1();
                            uart->writeChar(c);
                        }
                    }
                }
        }
    }
}
void MainWindow::on_lineEdit_returnPressed()
{
    on_pushButton_clicked();
}
void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if(item->backgroundColor() == Qt::darkMagenta)
    {
        item->setTextColor(Qt::black);
        item->setBackgroundColor(Qt::transparent);
    }
    else
    {
        item->setTextColor(Qt::white);
        item->setBackgroundColor(Qt::darkMagenta);
    }
}
void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    if(arg1.size() > 0)
    {
        //char c = arg1.at(0).toLatin1();
        //if( (c > 47) && (c < 58) ){
            //if(arg1.length() > 1) ui->lineEdit->setText(arg1.right(1));
        //}else
            //if(arg1.length() > 1) ui->lineEdit->setText(arg1.right(1));
    }
}
void MainWindow::onRightClick(QPoint *pos)
{
    QListWidgetItem* item = ui->listWidget->itemAt(*pos);
    item = ui->listWidget->takeItem(ui->listWidget->row(item));
    delete item;
}

void MainWindow::on_comboBox_activated(const QString &comboBoxEntryArg)
{
    if(ui->comboBox_2->currentText() != "Choose device")
    {
        if(comboBoxEntryArg != "Add Order")
        {
            OrderViewItem* ordItem;
            if(ui->comboBox_2->currentText() == "Forklift")
            {
                ordItem = new OrderViewItem(forkliftOrders, comboBoxEntryArg);
                forkliftOrders++;
            }else if(ui->comboBox_2->currentText() == "Gantry")
            {
                ordItem = new OrderViewItem(gantryOrders, comboBoxEntryArg, true);
                gantryOrders++;
            }
            ordersCreationScene->addItem(ordItem);
            if(ui->comboBox_2->currentText() == "Forklift")
            {
                if(orderItemsForklift.size() > 0)
                {
                    QPointF lastPos = orderItemsForklift.last()->pos();
                    ordItem->setPos(lastPos.x(), lastPos.y() + 100);
                    ordItem->setPrevious(orderItemsForklift.last());
                    OrderViewItem* theLastRealPointer = dynamic_cast<OrderViewItem*> (orderItemsForklift.last());
                    theLastRealPointer->setNextOrder(ordItem);
                }else
                {
                    ordItem->setPrevious(forkliftStart);
                    ordItem->setPos(forkliftStart->rect().x()-ordItem->rect().width()/2, forkliftStart->rect().y() );
                }
                orderItemsForklift.append(ordItem);
            }else if(ui->comboBox_2->currentText() == "Gantry")
            {
                if(orderItemsGantry.size() > 0)
                {
                    QPointF lastPos = orderItemsGantry.last()->pos();
                    ordItem->setPos(lastPos.x(), lastPos.y() + 100);
                    ordItem->setPrevious(orderItemsGantry.last());
                    OrderViewItem* theLastRealPointer = dynamic_cast<OrderViewItem*> (orderItemsGantry.last());
                    theLastRealPointer->setNextOrder(ordItem);
                }else
                {
                    ordItem->setPrevious(gantryStart);
                    ordItem->setPos(gantryStart->rect().x()-ordItem->rect().width()/2, +gantryStart->rect().y());
                }
                orderItemsGantry.append(ordItem);
            }
            ordItem->updateLines();
        }
    }else
    {
        QMessageBox::information(this, "Information","Select the desired device at first!");
    }
    ui->comboBox->setCurrentIndex(0);
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_comboBox_2_currentIndexChanged(const QString &arg1)
{

}
void MainWindow::saveOrders()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Open File"),
                                                    QCoreApplication::applicationDirPath()
                                                    ,"All files (*.*);; Save file (*.dat)");
    if( !fileName.isEmpty() )
    {
        QFile file(fileName);
        file.open(QIODevice::WriteOnly);
        QDataStream* stream = new QDataStream(&file);

        (*stream)<<orderItemsGantry.size();

        foreach(OrderViewItem* order, orderItemsGantry)
        {
            order->getData(stream);
        }
        (*stream)<<orderItemsForklift.size();

        foreach(OrderViewItem* order, orderItemsForklift)
        {
            order->getData(stream);
        }
        delete stream;
    }
}
void MainWindow::saveData()
{
    saveOrders();
}
void MainWindow::loadData()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"),
                                                    QCoreApplication::applicationDirPath()
                                                   ,"All files (*.*);; Save file (*.dat)");
    if( !fileName.isEmpty() )
    {
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        QDataStream* stream = new QDataStream(&file);

        int orderItemsGantrySize;
        (*stream)>>orderItemsGantrySize;

        for(int i=0; i<orderItemsGantrySize; i++)
        {
            OrderViewItem* order = new OrderViewItem(true);
            ordersCreationScene->addItem(order);
            order->loadData(stream);

            if(orderItemsGantry.size() > 0)
            {
                //QPointF lastPos = orderItemsGantry.last()->pos();
                //ordItem->setPos(lastPos.x(), lastPos.y() + 100);
                order->setPrevious(orderItemsGantry.last());
                OrderViewItem* theLastRealPointer = dynamic_cast<OrderViewItem*> (orderItemsGantry.last());
                theLastRealPointer->setNextOrder(order);
            }else
            {
                order->setPrevious(gantryStart);
                //ordItem->setPos(forkliftStart->rect().x()-ordItem->rect().width()/2, forkliftStart->rect().y() );
            }

            orderItemsGantry.append(order);
        }

        int orderItemsForkliftSize;
        (*stream)>>orderItemsForkliftSize;
        for(int i=0; i<orderItemsForkliftSize; i++)
        {
            OrderViewItem* order = new OrderViewItem();
            ordersCreationScene->addItem(order);
            order->loadData(stream);

            if(orderItemsForklift.size() > 0)
            {
                //QPointF lastPos = orderItemsGantry.last()->pos();
                //ordItem->setPos(lastPos.x(), lastPos.y() + 100);
                order->setPrevious(orderItemsForklift.last());
                OrderViewItem* theLastRealPointer = dynamic_cast<OrderViewItem*> (orderItemsForklift.last());
                theLastRealPointer->setNextOrder(order);
            }else
            {
                order->setPrevious(forkliftStart);
                //ordItem->setPos(forkliftStart->rect().x()-ordItem->rect().width()/2, forkliftStart->rect().y() );
            }

            orderItemsForklift.append(order);
        }


        foreach(OrderViewItem* item, orderItemsGantry)
        {
            if(item->savedBarrierOrderIndex != -1)
            {
                chainBarrierFromList(item, orderItemsGantry);
                item->barrierButton->setVisible(false);
            }
            if(item->savedContinueOrderIndex != -1)
            {
                chainContinueFromList(item, orderItemsGantry);
                item->continueButton->setVisible(false);
            }
        }
        foreach(OrderViewItem* item, orderItemsForklift)
        {
            if(item->savedBarrierOrderIndex != -1)
            {
                chainBarrierFromList(item, orderItemsForklift);
                item->barrierButton->setVisible(false);
            }
            if(item->savedContinueOrderIndex != -1)
            {
                chainContinueFromList(item, orderItemsForklift);
                item->continueButton->setVisible(false);
            }
        }

        foreach(OrderViewItem* item, orderItemsGantry)
        {
            item->updateLines();
        }
        foreach(OrderViewItem* item, orderItemsForklift)
        {
            item->updateLines();
        }
        delete stream;
    }
}
void MainWindow::chainBarrierFromList(OrderViewItem* itemArg, QList<OrderViewItem*>& orderItemsList)
{
    foreach(OrderViewItem* subItem, orderItemsList)
    {
        if(subItem->orderIndex == itemArg->savedBarrierOrderIndex)
        {
            itemArg->chainBarrierOrder(subItem);
            subItem->inverseChainBarrierOrder(itemArg);

            QPolygonF polRaw;
            polRaw.append(QPointF(0,0));
            polRaw.append(QPointF(-12.5,1.732*12.5));
            polRaw.append(QPointF(12.5,1.732*12.5));
            QGraphicsPolygonItem* poli = itemArg->scene()->addPolygon(polRaw);
            QGraphicsLineItem* line = itemArg->scene()->addLine( itemArg->getBarrierLineSchema() );
            itemArg->linkLineBarrier(line);

            QGraphicsSimpleTextItem* n = itemArg->scene()->addSimpleText("Barrier");
            n->setParentItem(line);
            n->setZValue(2);
            line->setZValue(-1);
            poli->setParentItem(line);
            poli->setZValue(3);
            poli->setBrush(Qt::white);

            break;
        }
    }
}
void MainWindow::chainContinueFromList(OrderViewItem* itemArg, QList<OrderViewItem*>& orderItemsList)
{
    foreach(OrderViewItem* subItem, orderItemsList)
    {
        if(subItem->orderIndex == itemArg->savedContinueOrderIndex)
        {
            itemArg->chainContinueOrder(subItem);
            subItem->inverseChainContinueOrder(itemArg);

            QPolygonF polRaw;
            polRaw.append(QPointF(0,0));
            polRaw.append(QPointF(-12.5,1.732*12.5));
            polRaw.append(QPointF(12.5,1.732*12.5));
            QGraphicsPolygonItem* poli = itemArg->scene()->addPolygon(polRaw);
            QGraphicsLineItem* line = itemArg->scene()->addLine( itemArg->getContinueLineSchema() );
            itemArg->linkLineContinue(line);

            QGraphicsSimpleTextItem* n = itemArg->scene()->addSimpleText("Continuous");
            n->setParentItem(line);
            poli->setParentItem(line);
            n->setZValue(2);
            line->setZValue(-1);
            poli->setZValue(3);
            poli->setBrush(Qt::white);

            break;
        }
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_pushButton_6_clicked()
{
    on_pushButton_4_clicked();
}

void MainWindow::on_pushButton_7_clicked()
{
    QString overall;
    overall.append('X');
    for(int i=0; i<orderItemsForklift.size(); i++)
    {
        QString toSend = orderItemsForklift.at(i)->prepareData();
        toSend.append('.');

        overall.append(toSend);
    }
    overall.append('_');
    for(int i=0; i<orderItemsGantry.size(); i++)
    {
        QString toSend = orderItemsGantry.at(i)->prepareData();
        toSend.append('.');

        overall.append(toSend);
    }
    overall.append('Y');

    for(int i=0; i<overall.size(); i++)
    {
        uart->writeChar(overall.at(i).toLatin1());
    }

    qDebug()<<overall;
}
