#include "QGraphicView_Demo.h"
#include "ui_QGraphicView_Demo.h"
#include "qgraphicsscene.h"
#include "qgraphicsitem.h"
#include "qgraphicsview.h"
#include "GraphicsControl.h"
#include "qdialog.h"
#include "qmessagebox.h"

QGraphicView_Demo::QGraphicView_Demo(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    m_control.SetView(ui.graphicsView);
    connect(ui.pushButton, &QPushButton::clicked, this, &QGraphicView_Demo::on_pushButton_clicked);
    try
    {
        this->show();
    }
    catch (std::exception& ex) {
        QMessageBox::warning(this, "error", ex.what(), QMessageBox::Ok);
    }
}

QGraphicView_Demo::~QGraphicView_Demo()
{}
void QGraphicView_Demo::on_pushButton_clicked() {
    try
    {

        ViewContent content1;
        content1.setTitle("test1");
        std::map<QString, QString> items1;
        items1.insert(items1.begin(), std::pair< QString, QString>(QString{ "test2" }, QString{ "test2" }));
        items1.insert(items1.begin(), std::pair< QString, QString>(QString{ "test3" }, QString{ "test4" }));
        items1.insert(items1.begin(), std::pair< QString, QString>(QString{ "test5" }, QString{ "test6" }));
        items1.insert(items1.begin(), std::pair< QString, QString>(QString{ "test7" }, QString{ "test8" }));
        items1.insert(items1.begin(), std::pair< QString, QString>(QString{ "test9" }, QString{ "test10" }));
        items1.insert(items1.begin(), std::pair< QString, QString>(QString{ "test11" }, QString{ "test12" }));
        content1.setContent(items1);

        ViewContent content2;
        content2.setTitle("test2");
        std::map<QString, QString> items2;
        items2.insert(items2.begin(), std::pair< QString, QString>(QString{ "test1" }, QString{ "test2" }));
        items2.insert(items2.begin(), std::pair< QString, QString>(QString{ "test3" }, QString{ "test4" }));
        items2.insert(items2.begin(), std::pair< QString, QString>(QString{ "test5" }, QString{ "test6" }));
        items2.insert(items2.begin(), std::pair< QString, QString>(QString{ "test7" }, QString{ "test8" }));
        items2.insert(items2.begin(), std::pair< QString, QString>(QString{ "test9" }, QString{ "test10" }));
        items2.insert(items2.begin(), std::pair< QString, QString>(QString{ "test11" }, QString{ "test12" }));
        content2.setContent(items2);

        ViewContent content3;
        content3.setTitle("test3");
        std::map<QString, QString> items3;
        items3.insert(items3.begin(), std::pair< QString, QString>(QString{ "test1" }, QString{ "test2" }));
        items3.insert(items3.begin(), std::pair< QString, QString>(QString{ "test3" }, QString{ "test4" }));    
        items3.insert(items3.begin(), std::pair< QString, QString>(QString{ "test5" }, QString{ "test6" }));
        items3.insert(items3.begin(), std::pair< QString, QString>(QString{ "test7" }, QString{ "test8" }));
        items3.insert(items3.begin(), std::pair< QString, QString>(QString{ "test9" }, QString{ "test10" }));
        content3.setContent(items3);

        std::vector<ViewContent> lists;
        lists.push_back(std::move(content1));
        lists.push_back(std::move(content2));
        lists.push_back(std::move(content3));

        m_control.Init(lists); 
        
        m_control.Render();
    }
    catch (std::exception& ex) {
        QMessageBox::warning(this, "error", ex.what(), QMessageBox::Ok);
    }
}
