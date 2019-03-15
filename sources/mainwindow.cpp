#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "battleitem.h"
#include "optionsdlg.h"
#include "picturescontainer.h"

#include <QFileDialog>
#include <QCloseEvent>
#include <QMessageBox>
#include <QBuffer>
#include <QtPrintSupport>

MainWindow::MainWindow(QString file, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);

    readSettings();

    QAction* recentFileAction = 0;
    for(auto i = 0; i < MaxRecentFiles; ++i){
        recentFileAction = new QAction(this);
        recentFileAction->setVisible(false);
        connect(recentFileAction,SIGNAL(triggered()),this, SLOT(openRecentFile()));
        recentFileActionList.append(recentFileAction);
    }

    m_recentMenu = new QMenu(this);

    ui->actionRecent->setMenu(m_recentMenu);
    for(auto i = 0; i < MaxRecentFiles; ++i)
        m_recentMenu->addAction(recentFileActionList.at(i));

    updateRecentFileActions();

    m_fileIsModified = false;
    m_currentFileName = "";

    m_container = new PicturesContainer(this);

    m_lastOpenDir = "";
    m_lastSavedDir = "";

    m_refreshCornerOnClic = true;
    m_refreshContentOnClic = false;

    m_modeActionGroup = new QActionGroup(this);
    m_modeActionGroup->addAction(ui->actionEdit);
    m_modeActionGroup->addAction(ui->actionPlay);
    m_modeActionGroup->setExclusive(true);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableWidget->insertColumn(0);
    ui->tableWidget->insertRow(0);

    m_cornerItem = new BattleItem(m_container,this);
    m_cornerItem->setCheckable(false);
    m_cornerItem->setMaskable(false);
    ui->tableWidget->setCellWidget(0,0,m_cornerItem);

    connect(ui->actionChooseColumnPictures,SIGNAL(triggered()),this,SLOT(updateColumnPictures()));
    connect(ui->actionChooseLinePictures,SIGNAL(triggered()),this,SLOT(updateLinePictures()));
    connect(ui->actionChooseCornerPictures,SIGNAL(triggered()),this,SLOT(updateCornerPictures()));
    connect(ui->actionChooseContentPictures,SIGNAL(triggered()),this,SLOT(updateContentPictures()));
    connect(ui->actionRemoveCornerPictures,SIGNAL(triggered()),this,SLOT(removeCornerPictures()));
    connect(ui->actionRemoveContentPictures,SIGNAL(triggered()),this,SLOT(removeContentPictures()));
    connect(ui->actionChooseLeftPicture,SIGNAL(triggered()),this,SLOT(updateLeftPicture()));
    connect(ui->actionRemoveLeftPicture,SIGNAL(triggered()),this,SLOT(removeLeftPicture()));
    connect(ui->actionChooseTopPicture,SIGNAL(triggered()),this,SLOT(updateTopPicture()));
    connect(ui->actionRemoveTopPicture,SIGNAL(triggered()),this,SLOT(removeTopPicture()));
    connect(ui->actionRefresh,SIGNAL(triggered()),this,SLOT(refresh()));
    connect(ui->actionOptions,SIGNAL(triggered()),this,SLOT(showOptionsDLG()));
    connect(m_modeActionGroup,SIGNAL(triggered(QAction *)),this,SLOT(changeMode()));
    connect(ui->actionSave,SIGNAL(triggered()),this,SLOT(save()));
    connect(ui->actionSaveAs,SIGNAL(triggered()),this,SLOT(saveAs()));
    connect(ui->actionQuit,SIGNAL(triggered()),this,SLOT(close()));
    connect(ui->actionNew,SIGNAL(triggered()),this,SLOT(newFile()));
    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(openFile()));
    connect(ui->actionPrint,SIGNAL(triggered()),this,SLOT(print()));

    if ( ! file.isEmpty() )
        openFile(file);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::readSettings(){
    QSettings settings("Yan", "Battleship");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    m_recentFilesList = settings.value("recentFileList").toStringList();
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action){
        if ( QFile(action->data().toString()).exists() ){
            openFile(action->data().toString());
        } else {
            QMessageBox msgBox;
            msgBox.setText(tr("Fichier introuvable."));
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
        }
    }
}

void MainWindow::updateRecentFileActions()
{
    auto itEnd = 0;
    if(m_recentFilesList.size() <= MaxRecentFiles)
        itEnd = m_recentFilesList.size();
    else
        itEnd = MaxRecentFiles;

    for (auto i = 0; i < itEnd; ++i) {
        QString strippedName = QFileInfo(m_recentFilesList.at(i)).fileName();
        recentFileActionList.at(i)->setText(strippedName);
        recentFileActionList.at(i)->setData(m_recentFilesList.at(i));
        recentFileActionList.at(i)->setVisible(true);
    }

    for (auto i = itEnd; i < MaxRecentFiles; ++i)
        recentFileActionList.at(i)->setVisible(false);

    ui->actionRecent->setEnabled(m_recentFilesList.count());
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::print(){
    QPrinter printer;
    printer.setResolution(1200);

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Imprimer"));
    if (dialog->exec() != QDialog::Accepted)
        return;

    QPainter painter;
    painter.begin(&printer);
    painter.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing|QPainter::LosslessImageRendering,true);
    painter.save();
    double xscale = printer.pageRect().width()/double(ui->centralWidget->width());
    double yscale = printer.pageRect().height()/double(ui->centralWidget->height());
    double scale = qMin(xscale, yscale);
    /*painter.translate(printer.paperRect().x() + printer.pageRect().width()/2,
                      printer.paperRect().y() + printer.pageRect().height()/2);
    painter.scale(scale, scale);
    painter.translate(-width()/2, -height()/2);

    ui->centralWidget->render(&painter);*/

    painter.translate(printer.paperRect().x() + printer.pageRect().width()/2,0);
    painter.scale(scale, scale);
    painter.translate(-width()/2, 0);

    ui->centralWidget->render(&painter);

    painter.restore();
    painter.translate(printer.paperRect().x() + printer.pageRect().width()/2,0);
    painter.scale(scale, scale);
    painter.translate(-width()/2,height());

    ui->centralWidget->render(&painter);
}

void MainWindow::openFile(){
    if ( m_fileIsModified ){
        bool acceptClose = false;
        QMessageBox msgBox;
        msgBox.setText(tr("Le fichier a été modifié."));
        msgBox.setInformativeText(tr("Voulez vous enregistrer les changements?"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret) {
        case QMessageBox::Save:{
            if ( save() == 0 )
               acceptClose = true;
            break;}
        case QMessageBox::Discard:
            acceptClose = true;
            break;
        }
        if ( ! acceptClose )
            return;
    }

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("BattleShip files (*.battleship)"));
    dialog.setViewMode(QFileDialog::Detail);
    if ( m_lastOpenDir.isEmpty() )
        dialog.setDirectory(QDir::homePath());
    else
        dialog.setDirectory(m_lastOpenDir);
    if (dialog.exec()){
        clearGame();

        m_lastOpenDir = dialog.directory().path();
        openFile(dialog.selectedFiles().at(0));
    }
}

void MainWindow::openFile(QString file){
    QString fileName = file;

    QFile f(fileName);
    f.open(QIODevice::ReadOnly);
    QDataStream in(&f);

    QPixmap pm;
    in >> pm;
    ui->leftLabel->setPixmap(pm);
    ui->actionRemoveLeftPicture->setEnabled(ui->leftLabel->pixmap());

    in >> pm;
    ui->topLabel->setPixmap(pm);
    ui->actionRemoveTopPicture->setEnabled(ui->topLabel->pixmap());

    int count;
    in >> count;

    for (int i = 0; i < count; i++){
        in >> pm;
        m_container->cornerPixmaps.append(pm);
    }
    if ( m_container->cornerPixmaps.count() ){
        m_cornerItem->setPixmap(PicturesContainer::CornerContainer,0);
        ui->actionRemoveCornerPictures->setEnabled(true);
    }else{
        m_cornerItem->setPixmap(PicturesContainer::CornerContainer,-1);
        ui->actionRemoveCornerPictures->setEnabled(false);
    }
    m_cornerItem->refresh();

    in >> count;
    ui->tableWidget->setColumnCount(count+1);

    for (int i = 0; i < count; i++){
        in >> pm;
        m_container->columnPixmaps.append(pm);

        BattleItem *item = new BattleItem(m_container,this);
        item->setCheckable(false);
        item->setMaskable(false);
        item->setPixmap(PicturesContainer::ColumnContainer,i);

        ui->tableWidget->setCellWidget(0,i+1,item);

        for (int j = 1; j < ui->tableWidget->rowCount(); j++){
            BattleItem *item = new BattleItem(m_container,this);
            item->setCheckable(true);
            item->setMaskable(true);
            connect(item,SIGNAL(clicked()),this,SLOT(onClick()));
            connect(item,SIGNAL(checkedModified()),this,SLOT(onCheckModified()));

            ui->tableWidget->setCellWidget(j,i+1,item);
        }
    }

    in >> count;
    ui->tableWidget->setRowCount(count+1);

    for( int i = 0; i < count; i++){
        in >> pm;
        m_container->linePixmaps.append(pm);

        BattleItem *item = new BattleItem(m_container,this);
        item->setCheckable(false);
        item->setMaskable(false);
        item->setPixmap(PicturesContainer::LineContainer,i);

        ui->tableWidget->setCellWidget(i+1,0,item);

        for (int j = 1; j < ui->tableWidget->columnCount(); j++){
            BattleItem *item = new BattleItem(m_container,this);
            item->setCheckable(true);
            item->setMaskable(true);
            connect(item,SIGNAL(clicked()),this,SLOT(onClick()));
            connect(item,SIGNAL(checkedModified()),this,SLOT(onCheckModified()));

            ui->tableWidget->setCellWidget(i+1,j,item);
        }
    }

    in >> count;

    if ( count ){
        ui->actionRemoveContentPictures->setEnabled(true);

        for(int i = 0; i < count; i++ ){
            in >> pm;
            m_container->contentPixmaps.append(pm);
        }

        for(int i = 1; i < ui->tableWidget->columnCount(); i++){
            for(int j = 1; j < ui->tableWidget->rowCount(); j++ ){
                BattleItem *it = qobject_cast<BattleItem *>( ui->tableWidget->cellWidget(j,i));
                if (it){
                    it->setMaskPixmap(PicturesContainer::ContentContainer,0);
                    it->refresh();
                }
            }
        }
    }

    for (int i = 1; i < ui->tableWidget->columnCount(); i++ ){
        for (int j = 1; j < ui->tableWidget->rowCount(); j++ ){
            BattleItem *it = qobject_cast<BattleItem *>( ui->tableWidget->cellWidget(j,i));
            if (it){
                bool isChecked;
                in >> isChecked;
                it->setChecked(isChecked);
            }
        }
    }

    m_currentFileName = fileName;
    m_fileIsModified = false;

    QFileInfo fi(m_currentFileName);
    QString shortName = fi.baseName();
    setWindowTitle(QString("BattleShip - %1").arg(shortName));

    ui->actionPlay->setChecked(true);
    changeMode();

    updateRecentFiles();
}

void MainWindow::updateRecentFiles(){
    QSettings settings("Yan", "Battleship");
    QStringList recentFilePaths = settings.value("recentFileList").toStringList();
    recentFilePaths.removeAll(m_currentFileName);
    recentFilePaths.prepend(m_currentFileName);
    while (recentFilePaths.size() > MaxRecentFiles)
        recentFilePaths.removeLast();
    settings.setValue("recentFileList", recentFilePaths);
    m_recentFilesList = recentFilePaths;

    updateRecentFileActions();
}

void MainWindow::newFile(){
    if ( m_fileIsModified ){
        bool acceptClose = false;
        QMessageBox msgBox;
        msgBox.setText(tr("Le fichier a été modifié."));
        msgBox.setInformativeText(tr("Voulez vous enregistrer les changements?"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret) {
        case QMessageBox::Save:{
            if ( save() == 0 )
               acceptClose = true;
            break;}
        case QMessageBox::Discard:
            acceptClose = true;
            break;
        }
        if ( ! acceptClose )
            return;
    }
    clearGame();
    ui->actionEdit->setChecked(true);
    changeMode();
}

void MainWindow::clearGame(){
    ui->tableWidget->setColumnCount(1);
    ui->tableWidget->setRowCount(1);
    m_container->columnPixmaps.clear();
    m_container->linePixmaps.clear();
    m_container->contentPixmaps.clear();
    m_container->cornerPixmaps.clear();
    m_cornerItem->setPixmap(PicturesContainer::CornerContainer,-1);
    ui->topLabel->setPixmap(QPixmap());
    ui->leftLabel->setPixmap(QPixmap());
    m_fileIsModified = false;
    m_currentFileName = "";
    setWindowTitle("Battleship - New");
    ui->actionRemoveContentPictures->setEnabled(false);
    ui->actionRemoveCornerPictures->setEnabled(false);
    ui->actionRemoveTopPicture->setEnabled(false);
    ui->actionRemoveLeftPicture->setEnabled(false);
}

void MainWindow::closeEvent(QCloseEvent *event){
    if ( m_fileIsModified ){
        QMessageBox msgBox;
        msgBox.setText(tr("Le fichier a été modifié."));
        msgBox.setInformativeText(tr("Voulez vous enregistrer les changements?"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret) {
            case QMessageBox::Save:{
                if ( save() == 0 )
                    event->accept();
                else
                    event->ignore();
                break;}
            case QMessageBox::Discard:
                event->accept();
                break;
            case QMessageBox::Cancel:
                event->ignore();
                break;
          }
    }else{
        QSettings settings("Yan", "Battleship");
        settings.setValue("geometry", saveGeometry());
        settings.setValue("windowState", saveState());
        event->accept();
    }
}

int MainWindow::save(){
    if ( m_currentFileName.isEmpty() ){
        return saveAs();
    }else{
        saveFile(m_currentFileName);
        return 0;
    }
}

int MainWindow::saveAs(){
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("BattleShip files (*.battleship)"));
    dialog.setViewMode(QFileDialog::Detail);
    if ( m_lastSavedDir.isEmpty() )
        dialog.setDirectory(QDir::homePath());
    else
        dialog.setDirectory(m_lastSavedDir);
    if (dialog.exec()){
        m_lastSavedDir = dialog.directory().path();
        saveFile(dialog.selectedFiles().at(0));
        return 0;
    }else{
        return 1;
    }
}

void MainWindow::saveFile(QString fileName){
    if ( ! fileName.endsWith(".battleship"))
        fileName.append(".battleship");

    m_currentFileName = fileName;

    QFileInfo fi(m_currentFileName);
    QString shortName = fi.baseName();

    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);

    QPixmap pm;

    if ( ui->leftLabel->pixmap() ){
        pm = *ui->leftLabel->pixmap();
        out << pm;
    }else{
        out << QPixmap();
    }

    if ( ui->topLabel->pixmap() ){
        pm = *ui->topLabel->pixmap();
        out << pm;
    }else{
        out << QPixmap();
    }

    out << m_container->cornerPixmaps.length();
    for (int i = 0; i < m_container->cornerPixmaps.length(); i++)
        out << m_container->cornerPixmaps.at(i);

    out << m_container->columnPixmaps.length();
    for (int i = 0; i < m_container->columnPixmaps.length(); i++)
        out << m_container->columnPixmaps.at(i);

    out << m_container->linePixmaps.length();
    for (int i = 0; i < m_container->linePixmaps.length(); i++)
        out << m_container->linePixmaps.at(i);

    out << m_container->contentPixmaps.length();
    for (int i = 0; i < m_container->contentPixmaps.length(); i++)
        out << m_container->contentPixmaps.at(i);

    for (int i = 1; i < ui->tableWidget->columnCount(); i++ ){
        for (int j = 1; j < ui->tableWidget->rowCount(); j++ ){
            BattleItem *it = qobject_cast<BattleItem *>( ui->tableWidget->cellWidget(j,i));
            if (it)
                out << it->isChecked();
        }
    }

    setWindowTitle(QString("BattleShip - %1").arg(shortName));
    m_fileIsModified = false;

    updateRecentFiles();
}

void MainWindow::onCheckModified(){
    m_fileIsModified = true;
    if ( ! windowTitle().endsWith("*") )
        setWindowTitle(windowTitle() + " *");
}

void MainWindow::updateLeftPicture(){
    QStringList picturesList = getSelectedFiles(false);

    if ( picturesList.count() ){
        QPixmap pm = QPixmap(picturesList.at(0));
        ui->leftLabel->setPixmap(pm.scaled(48,48,Qt::KeepAspectRatio));
        ui->actionRemoveLeftPicture->setEnabled(true);
    }
}

void MainWindow::removeLeftPicture(){
    m_fileIsModified = true;
    if ( ! windowTitle().endsWith("*") )
        setWindowTitle(windowTitle() + " *");
    ui->leftLabel->setPixmap(QPixmap());
    ui->actionRemoveLeftPicture->setEnabled(false);
}

void MainWindow::updateTopPicture(){
    QStringList picturesList = getSelectedFiles(false);

    if ( picturesList.count() ){
        QPixmap pm = QPixmap(picturesList.at(0));
        ui->topLabel->setPixmap(pm.scaled(48,48,Qt::KeepAspectRatio));
        ui->actionRemoveTopPicture->setEnabled(true);
    }
}

void MainWindow::removeTopPicture(){
    m_fileIsModified = true;
    if ( ! windowTitle().endsWith("*") )
        setWindowTitle(windowTitle() + " *");
    ui->topLabel->setPixmap(QPixmap());
    ui->actionRemoveTopPicture->setEnabled(false);
}

void MainWindow::showOptionsDLG(){
    OptionsDLG *dlg = new OptionsDLG(m_refreshCornerOnClic,m_refreshContentOnClic,this);
    if ( dlg->exec() == QDialog::Accepted ){
        m_refreshCornerOnClic = dlg->refreshCorner();
        m_refreshContentOnClic = dlg->refreshContent();
    }
}

void MainWindow::refresh(){
    m_cornerItem->refresh();
    for(int i = 1; i < ui->tableWidget->columnCount(); i++){
        for(int j = 1; j < ui->tableWidget->rowCount(); j++ ){
            BattleItem *it = qobject_cast<BattleItem *>( ui->tableWidget->cellWidget(j,i));
            if (it){
                it->refresh();
                if ( ui->actionPlay->isChecked() )
                    it->setMasked(true);
            }
        }
    }
}

void MainWindow::changeMode(){
    if ( ui->actionEdit->isChecked() )
        ui->menuChoosePictures->setEnabled(true);
    else
        ui->menuChoosePictures->setEnabled(false);

    for(int i = 1; i < ui->tableWidget->columnCount(); i++){
        for(int j = 1; j < ui->tableWidget->rowCount(); j++ ){
            BattleItem *it = qobject_cast<BattleItem *>( ui->tableWidget->cellWidget(j,i));
            if (it){
                if ( ui->actionEdit->isChecked() ){
                    it->setMode(BattleItem::Edit);
                    it->setMasked(false);
                }else{
                    it->setMode(BattleItem::Normal);
                    it->setMasked(true);
                }
            }
        }
    }
}

QStringList MainWindow::getSelectedFiles(bool multipleSelection){
    QStringList fileList;
    QFileDialog dialog(this);
    if ( multipleSelection )
        dialog.setFileMode(QFileDialog::ExistingFiles);
    else
        dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.gif *.jpg)"));
    dialog.setViewMode(QFileDialog::Detail);
    if ( m_lastOpenDir.isEmpty() )
        dialog.setDirectory(QDir::homePath());
    else
        dialog.setDirectory(m_lastOpenDir);
    if (dialog.exec()){
        m_lastOpenDir = dialog.directory().path();
        fileList = dialog.selectedFiles();
        m_fileIsModified = true;
        if ( ! windowTitle().endsWith("*") )
            setWindowTitle(windowTitle() + " *");
    }
    return fileList;
}

void MainWindow::updateColumnPictures(){
    QStringList picturesList = getSelectedFiles();

    if ( picturesList.count() ){
        for(int i = ui->tableWidget->columnCount() - 1; i > 0 ; i -- )
            ui->tableWidget->removeColumn(i);

        ui->tableWidget->setColumnCount(picturesList.length()+1);

        m_container->columnPixmaps.clear();

        for( int i = 0; i < picturesList.length(); i++){
            m_container->columnPixmaps.append(QPixmap(picturesList.at(i)).scaled(256,256,Qt::KeepAspectRatio));

            BattleItem *item = new BattleItem(m_container,this);
            item->setCheckable(false);
            item->setMaskable(false);
            item->setPixmap(PicturesContainer::ColumnContainer,i);

            ui->tableWidget->setCellWidget(0,i+1,item);

            for (int j = 1; j < ui->tableWidget->rowCount(); j++){
                BattleItem *item = new BattleItem(m_container,this);
                item->setCheckable(true);
                item->setMaskable(true);
                connect(item,SIGNAL(clicked()),this,SLOT(onClick()));
                connect(item,SIGNAL(checkedModified()),this,SLOT(onCheckModified()));

                ui->tableWidget->setCellWidget(j,i+1,item);
            }
        }
    }
}

void MainWindow::updateLinePictures(){
    QStringList picturesList = getSelectedFiles();

    if ( picturesList.count() ){

        for(int i = ui->tableWidget->rowCount() - 1; i > 0 ; i -- )
            ui->tableWidget->removeRow(i);

        ui->tableWidget->setRowCount(picturesList.length()+1);

        m_container->linePixmaps.clear();

        for( int i = 0; i < picturesList.length(); i++){
            m_container->linePixmaps.append(QPixmap(picturesList.at(i)).scaled(256,256,Qt::KeepAspectRatio));

            BattleItem *item = new BattleItem(m_container,this);
            item->setCheckable(false);
            item->setMaskable(false);
            item->setPixmap(PicturesContainer::LineContainer,i);

            ui->tableWidget->setCellWidget(i+1,0,item);

            for (int j = 1; j < ui->tableWidget->columnCount(); j++){
                BattleItem *item = new BattleItem(m_container,this);
                item->setCheckable(true);
                item->setMaskable(true);
                connect(item,SIGNAL(clicked()),this,SLOT(onClick()));
                connect(item,SIGNAL(checkedModified()),this,SLOT(onCheckModified()));

                ui->tableWidget->setCellWidget(i+1,j,item);
            }
        }
    }
}

void MainWindow::updateCornerPictures(){
    QStringList picturesList = getSelectedFiles();

    if ( picturesList.count() ){
        ui->actionRemoveCornerPictures->setEnabled(true);
        m_container->cornerPixmaps.clear();

        for(int i = 0; i < picturesList.length(); i++ )
            m_container->cornerPixmaps.append(QPixmap(picturesList.at(i)).scaled(256,256,Qt::KeepAspectRatio));

        m_cornerItem->setPixmap(PicturesContainer::CornerContainer,0);
        m_cornerItem->refresh();
    }
}

void MainWindow::updateContentPictures(){
    QStringList picturesList = getSelectedFiles();

    if ( picturesList.count() ){
        ui->actionRemoveContentPictures->setEnabled(true);
        m_container->contentPixmaps.clear();

        for(int i = 0; i < picturesList.length(); i++ )
            m_container->contentPixmaps.append(QPixmap(picturesList.at(i)).scaled(256,256,Qt::KeepAspectRatio));

        for(int i = 1; i < ui->tableWidget->columnCount(); i++){
            for(int j = 1; j < ui->tableWidget->rowCount(); j++ ){
                BattleItem *it = qobject_cast<BattleItem *>( ui->tableWidget->cellWidget(j,i));
                if (it){
                    it->setMaskPixmap(PicturesContainer::ContentContainer,0);
                    it->refresh();
                }
            }
        }
    }
}

void MainWindow::removeCornerPictures(){
    m_cornerItem->setPixmap(PicturesContainer::CornerContainer,-1);
    m_container->cornerPixmaps.clear();
    m_fileIsModified = true;
    if ( ! windowTitle().endsWith("*") )
        setWindowTitle(windowTitle() + " *");
    ui->actionRemoveCornerPictures->setEnabled(false);
}

void MainWindow::removeContentPictures(){
    m_fileIsModified = true;
    if ( ! windowTitle().endsWith("*") )
        setWindowTitle(windowTitle() + " *");
    ui->actionRemoveContentPictures->setEnabled(false);
    for(int i = 1; i < ui->tableWidget->columnCount(); i++){
        for(int j = 1; j < ui->tableWidget->rowCount(); j++ ){
            BattleItem *it = qobject_cast<BattleItem *>( ui->tableWidget->cellWidget(j,i));
            if (it){
                it->setMaskPixmap(PicturesContainer::ContentContainer,-1);
                it->refresh();
            }
        }
    }
    m_container->contentPixmaps.clear();
}

void MainWindow::onClick(){
    if ( m_refreshCornerOnClic )
        m_cornerItem->refresh();
    if ( m_refreshContentOnClic ){
        for(int i = 1; i < ui->tableWidget->columnCount(); i++){
            for(int j = 1; j < ui->tableWidget->rowCount(); j++ ){
                BattleItem *it = qobject_cast<BattleItem *>( ui->tableWidget->cellWidget(j,i));
                if (it){
                    it->refresh();
                }
            }
        }
    }
}
