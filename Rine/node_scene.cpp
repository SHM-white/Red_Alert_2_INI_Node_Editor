#include "node_scene.h"

node_scene::node_scene(QWidget *parent){
    //this->gridsize
    this->pen_light.setWidth(1);
    this->pen_dark.setWidth(2);
    this->setBackgroundBrush(QColor("#393939"));
    this->setSceneRect(-this->scene_width/2,-this->scene_height/2,this->scene_width,this->scene_height);
}

void node_scene::drawBackground(QPainter *painter, const QRectF &rect){
    QGraphicsScene::drawBackground(painter,rect);
    this->left=floor(rect.left());
    this->right=floor(rect.right());
    this->top=floor(rect.top());
    this->bottom=floor(rect.bottom());
    this->first_left=left-(left%this->gridSize);
    this->first_top=top-(top%this->gridSize);
    int x=0,y=0,i=0;
    for(x=this->first_left;x<this->right;x+=this->gridSize){
        if(x%100!=0){
            this->lines_light.append(QLine(x,this->top,x,this->bottom));
        }else{
            this->lines_dark.append(QLine(x,this->top,x,this->bottom));
        }
    }
    for(y=this->first_top;y<this->bottom;y+=this->gridSize){
        if(y%100!=0){
            this->lines_light.append(QLine(this->left,y,this->right,y));
        }else{
            this->lines_dark.append(QLine(this->left,y,this->right,y));
        }
    }
    painter->setPen(this->pen_light);
    for(i=0;i<this->lines_light.count();++i){
        painter->drawLine(this->lines_light[i]);
    }
    painter->setPen(this->pen_dark);
    for(i=0;i<this->lines_dark.count();++i){
        painter->drawLine(this->lines_dark[i]);
    }
}
