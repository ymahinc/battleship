#include "battleitem.h"
#include "ui_battleitem.h"

#include <QPainter>

BattleItem::BattleItem(PicturesContainer *container, QWidget *parent)
    : QWidget(parent), ui(new Ui::BattleItem), m_container(container){
    ui->setupUi(this);

    m_pixmapIndex = -1;
    m_maskPixmapIndex = -1;

    m_isCheckable = true;
    m_isChecked = false;
    m_isMaskable = true;
    m_isMasked = false;
    m_mode = Edit;
}

BattleItem::~BattleItem(){
    delete ui;
}

bool BattleItem::isChecked(){
    return m_isChecked;
}

void BattleItem::setChecked(bool checked){
    m_isChecked = checked;
    update();
}

void BattleItem::setPixmap(PicturesContainer::Category category, int index){
    m_pixmapIndex = index;
    m_category = category;
    update();
}

void BattleItem::setMaskPixmap(PicturesContainer::Category category, int index){
    m_maskPixmapIndex = index;
    m_category = category;
    update();
}

void BattleItem::refresh(){
    if ( m_maskPixmapIndex >= 0 ){
        m_maskPixmapIndex = qrand() % (m_container->contentPixmaps.length());
        update();
    }
    if ( m_pixmapIndex >= 0 ){
        switch ( m_category ){
            case PicturesContainer::ColumnContainer:
            m_pixmapIndex = qrand() % (m_container->columnPixmaps.length());
            break;
            case PicturesContainer::LineContainer:
            m_pixmapIndex = qrand() % (m_container->linePixmaps.length());
            break;
            case PicturesContainer::CornerContainer:
            m_pixmapIndex = qrand() % (m_container->cornerPixmaps.length());
            break;
            case PicturesContainer::ContentContainer:
            m_pixmapIndex = qrand() % (m_container->contentPixmaps.length());
            break;
        }
        update();
    }
}

void BattleItem::setMode(Mode mode){
    m_mode = mode;
}

void BattleItem::setCheckable(bool checkable){
    m_isCheckable = checkable;
}

void BattleItem::setMaskable(bool maskable){
    m_isMaskable = maskable;
}

void BattleItem::setMasked(bool masked){
    m_isMasked = masked;
    update();
}

void BattleItem::mousePressEvent(QMouseEvent *event){
    if ( m_mode == Normal ){
        if ( m_isMaskable )
            m_isMasked = ! m_isMasked;
    }
    if ( m_mode == Edit ){
        if ( m_isCheckable ){
            m_isChecked = !m_isChecked;
            emit checkedModified();
        }
    }

    emit clicked();

    update();
}

void BattleItem::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    if ( m_isChecked ){
        painter.drawPixmap(rect(),m_container->crossPixmap,m_container->crossPixmap.rect());
    }else{
        if ( m_pixmapIndex < 0 ){
            painter.fillRect(rect(),Qt::white);
        }else{
            switch ( m_category ){
                case PicturesContainer::ColumnContainer:
                painter.drawPixmap(rect(),m_container->columnPixmaps.at(m_pixmapIndex),m_container->columnPixmaps.at(m_pixmapIndex).rect());
                break;
                case PicturesContainer::LineContainer:
                painter.drawPixmap(rect(),m_container->linePixmaps.at(m_pixmapIndex),m_container->linePixmaps.at(m_pixmapIndex).rect());
                break;
                case PicturesContainer::CornerContainer:
                painter.drawPixmap(rect(),m_container->cornerPixmaps.at(m_pixmapIndex),m_container->cornerPixmaps.at(m_pixmapIndex).rect());
                break;
                case PicturesContainer::ContentContainer:
                painter.drawPixmap(rect(),m_container->contentPixmaps.at(m_pixmapIndex),m_container->contentPixmaps.at(m_pixmapIndex).rect());
                break;
            }
        }
    }
    if ( m_isMasked ){
        if ( m_maskPixmapIndex < 0 )
            painter.fillRect(rect(),Qt::gray);
        else
            painter.drawPixmap(rect(),m_container->contentPixmaps.at(m_maskPixmapIndex),m_container->contentPixmaps.at(m_maskPixmapIndex).rect());
    }
}
