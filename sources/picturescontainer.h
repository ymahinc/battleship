#ifndef PICTURESCONTAINER_H
#define PICTURESCONTAINER_H

#include <QObject>
#include <QVector>
#include <QPixmap>

class PicturesContainer : public QObject
{
    Q_OBJECT

public:
    enum Category { CornerContainer, ContentContainer, ColumnContainer, LineContainer };

    PicturesContainer(QObject *parent = nullptr);
    QVector<QPixmap> cornerPixmaps;
    QVector<QPixmap> contentPixmaps;
    QVector<QPixmap> columnPixmaps;
    QVector<QPixmap> linePixmaps;

    QPixmap crossPixmap = QPixmap(":/pictures/cross.png");
};

#endif // PICTURESCONTAINER_H
