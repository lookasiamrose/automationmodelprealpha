#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <uart.h>
#include "ordersgraphicsscene.h"
#include <QGraphicsView>

#include "orderviewitem.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void saveOrders();
    void chainBarrierFromList(OrderViewItem* itemArg, QList<OrderViewItem*>& orderItemsList);
    void chainContinueFromList(OrderViewItem* itemArg, QList<OrderViewItem*>& orderItemsList);
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_lineEdit_returnPressed();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void com_ports();
    void saveData();
    void loadData();

    void menuSelection(QAction*);
    void baudSelection(QAction*);
    void on_lineEdit_textChanged(const QString &arg1);

    void onRightClick(QPoint *pos);

    void on_comboBox_activated(const QString &arg1);

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_comboBox_2_currentIndexChanged(const QString &arg1);

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

private:
    Ui::MainWindow *ui;
    Uart* uart;
    QAction* newAct;
    QList<QAction*> actionPorts;
    QMenu *fileMenu;
    QMenu *subMenu;
    QMenu *baudMenu;
    QGraphicsScene* ordersCreationScene;

    QList<OrderViewItem*> orderItemsGantry;
    QList<OrderViewItem*> orderItemsForklift;

    void prepareFirstAppearance();

    QGraphicsRectItem* forkliftStart;
    QGraphicsRectItem* gantryStart;
    int forkliftOrders;
    int gantryOrders;
    QMenu *saveMenu;
};

#endif // MAINWINDOW_H
