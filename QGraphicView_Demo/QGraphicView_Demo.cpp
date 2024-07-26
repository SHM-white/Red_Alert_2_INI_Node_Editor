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
        items1.insert(items1.begin(), std::pair< QString, QString>(QString{ "test1" }, QString{ "test2" }));
        items1.insert(items1.begin(), std::pair< QString, QString>(QString{ "test3" }, QString{ "test4" }));
        content1.setContent(items1);

        ViewContent content2;
        content2.setTitle("test2");
        std::map<QString, QString> items2;
        items2.insert(items2.begin(), std::pair< QString, QString>(QString{ "test1" }, QString{ "test2" }));
        items2.insert(items2.begin(), std::pair< QString, QString>(QString{ "test3" }, QString{ "test4" }));
        content2.setContent(items2);

        std::vector<ViewContent> lists;
        lists.push_back(std::move(content1));
        lists.push_back(std::move(content2));
        m_control.Init(lists); // 使用成员变量 m_control
        m_control.Render();
    }
    catch (std::exception& ex) {
        QMessageBox::warning(this, "error", ex.what(), QMessageBox::Ok);
    }
}
