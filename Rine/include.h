#pragma once

#include "node_scene.h"
#include <QFile>
#include <QPainter>
#include <QRectF>
#include <QGraphicsView>
#include "GraphicsControl.h"
#include "mini/ini.h"
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QRectF>
#include <QLine>
#include <QWidget>
#include <QtCore>
#include <QtGui>
#include <QList>
#include <QMainWindow>
#include <QWheelEvent>
#include <QMouseEvent>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <filesystem>
#include <algorithm>
#include <ranges>
#include <qlistwidget.h>
#include <iterator>
#include <cctype>
#include <unordered_set>

#ifdef QT_DEBUG
#define DEBUG
#endif

//global vars
extern int VIEW_DISPLAY_DEPTH;


class Utils
{
public:
	static std::vector<ViewContent> GetRelatedContents(const ViewContent* content, const std::vector<ViewContent>& contents, int currentDepth = 0);
	static std::vector<ViewContent> RemoveRepeatedContents(const std::vector<ViewContent>& contents);
};