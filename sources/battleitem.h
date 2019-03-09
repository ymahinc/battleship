#ifndef BATTLEITEM_H
#define BATTLEITEM_H

#include "picturescontainer.h"

#include <QWidget>

namespace Ui {
class BattleItem;
}

class BattleItem : public QWidget
{
    Q_OBJECT

public:
    enum Mode { Normal, Edit };

    explicit BattleItem(PicturesContainer *container, QWidget *parent = nullptr);
    ~BattleItem();

    void setMode(Mode mode);
    void setCheckable(bool checkable);
    void setMaskable(bool maskable);
    void setMasked(bool masked);
    void setPixmap(PicturesContainer::Category category, int index);
    void setMaskPixmap(PicturesContainer::Category category, int index);
    void refresh();
    bool isChecked();
    void setChecked(bool checked);

signals:
    void clicked();
    void checkedModified();

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    Ui::BattleItem *ui;

    bool m_isCheckable;
    bool m_isChecked;
    bool m_isMaskable;
    bool m_isMasked;
    Mode m_mode;
    int m_pixmapIndex;
    int m_maskPixmapIndex;
    PicturesContainer::Category m_category;
    PicturesContainer *m_container;
};

#endif // BATTLEITEM_H
