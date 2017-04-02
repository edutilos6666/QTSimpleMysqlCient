#include "mainwindow.h"
#include <QApplication>
#include <QtSql>
#include <QString>
#include <QDebug>
#include <QTreeView>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStringList>
#include <QLabel>
#include <QTableView>


void testSQL() {
  QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL3");
  db.setHostName("localhost");
  db.setPort(3306);
  db.setUserName("root");
  db.setPassword("");
db.setDatabaseName("mysql");
  QSqlQuery query ;
  if(!db.open()) {
      qDebug() << "connection error" ;
  }
qDebug() << "connection success";

  query.prepare("show databases");
  query.exec();
  while(query.next()) {
      qDebug() << query.value(0).toString() ;
  }



  query.prepare("show tables");
  query.exec();
  qDebug() << "tables" ;
  while(query.next()) {
      qDebug() << query.value(0).toString();
  }


   db = QSqlDatabase::addDatabase("QMYSQL3");
   db.setHostName("localhost");
   db.setPort(3306);
   db.setUserName("root");
  db.setDatabaseName("test");
  if(!db.open()) return ;
   query = QSqlQuery();
  query.prepare("show tables");
  query.exec();
  qDebug() << "tables" ;
  while(query.next()) {
      qDebug() << query.value(0).toString();
  }

}


class SimpleMysqlClient {
public:
    SimpleMysqlClient()  {
        tableModel = new QStandardItemModel();
    }

//QStringList dbNames() ;
//QStringList tableNames(QString dbName) ;
 QTreeView *treeView ;
  QStandardItemModel *model ;
   QStandardItemModel *tableModel;
void onCustomContextMenuRequested(const QPoint &point) {
    QModelIndex index = treeView->indexAt(point);
    qDebug() << index.row() << ", " << index.column();
}

void testTreeView() {
  QMainWindow *window = new QMainWindow();
  window->resize(500, 500);
  window->setWindowTitle("TreeView Example");
  QWidget *widget = new QWidget(window);
  window->setCentralWidget(widget);
 QVBoxLayout *layout = new QVBoxLayout();
 widget->setLayout(layout);
 QLabel *lblTitle = new QLabel();
 lblTitle->setText("MysqlClient GUI");
  layout->addWidget(lblTitle);

  treeView  = new QTreeView(window);
    model =  new QStandardItemModel(window);
  treeView->setModel(model);
   QStringList parentStr = dbNames();
   for(auto db: parentStr)  {
       QStandardItem *parent = new QStandardItem();
       parent->setText(db);
       QStringList tableStr = tableNames(db);
       for(auto table:tableStr)  {
           QStandardItem *t =  new QStandardItem();
           t->setText(table);
           parent->appendRow(t);
       }

       model->appendRow(parent);
   }

   layout->addWidget(treeView);
/*
 *     enum ContextMenuPolicy {
        NoContextMenu,
        DefaultContextMenu,
        ActionsContextMenu,
        CustomContextMenu,
        PreventContextMenu
    };

 * */
   treeView->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
  // QObject::connect(treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on)
   QObject::connect(treeView, &QTreeView::customContextMenuRequested, [this](const QPoint &point){
       QModelIndex index = treeView->indexAt(point);

//       qDebug() << index.row() << ", " << index.column();
//      qDebug() << model->data(index).toString()  << ", " << model->data(index.parent()).toString();

         QString tableName = model->data(index).toString(),
                 dbName = model->data(index.parent()).toString();

         if(dbName != "" && tableName != "") {
              showTableContent(tableName, dbName);
         }

   });
   window->show();
}



void showTableContent(QString tableName, QString dbName) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL3");
    db.setHostName("localhost");
    db.setPassword("");
    db.setUserName("root");
    db.setDatabaseName(dbName);

    if(!db.open()) return ;
    QString cmd ;
    QTextStream out(&cmd);
    out << "select * from " << tableName ;
    QSqlQuery query ;
   query.prepare(cmd);
   query.exec();
   QVector<QVector<QString>> rs ;
  while(query.next()) {
    int size = query.record().count();
    QStringList colNames ;
    for(int i=0; i< size; ++i) {
        colNames << query.record().fieldName(i);
    }

    qDebug() << colNames;
    //((QStandardItemModel*)treeView->model())->setHorizontalHeaderLabels(colNames);
    tableModel->setHorizontalHeaderLabels(colNames);
    QVector<QString> list ;
    for(int i=0; i< size; ++i) {
        list << query.value(i).toString();
    }

    rs<< list;
  }

  qDebug() << rs ;
  db.close();

  QMainWindow *window = new QMainWindow();
  window->resize(500, 500);
  window->setWindowTitle("ResulSet");
  QVBoxLayout *layout  = new QVBoxLayout();
  QWidget *widget = new QWidget(window);
  window->setCentralWidget(widget);
  widget->setLayout(layout);
  QTableView *tableView = new QTableView();

  tableView->setModel(tableModel);
  layout->addWidget(tableView);

  int i=0 ;
  for(QVector<QString> v: rs) {
    QList<QStandardItem*> listItems ;
      for(QString str: v) {
          QStandardItem *item = new QStandardItem();
          item->setText(str);
         listItems.append(item);
      }
      tableModel->appendRow(listItems);
  }
  window->show();
}

QStringList tableNames(QString dbName) {
QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL3");
db.setHostName("localhost");
db.setPassword("");
db.setUserName("root");
db.setDatabaseName(dbName);
QStringList list ;
if(!db.open()) return list;
QSqlQuery query ;
query.prepare("show tables");
query.exec();
while(query.next()) {
    list << query.value(0).toString();
}
db.close();
return list;
}

QStringList dbNames() {
QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL3");
db.setHostName("localhost");
db.setUserName("root");
db.setPassword("");
QStringList list ;
if(!db.open()) return list ;
QSqlQuery query;
  query.prepare("show databases");
  query.exec();
  while(query.next()) {
      list << query.value(0).toString();
  }
db.close();
  return list ;
}


};
int main(int argc, char *argv[])
{
//    testSQL();
//    return 0;
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
    SimpleMysqlClient fy ;
    fy.testTreeView();
    return a.exec();
}
