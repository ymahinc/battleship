#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>

class BattleItem;
class PicturesContainer;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QString file, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateColumnPictures();
    void updateLinePictures();
    void changeMode();
    void updateCornerPictures();
    void updateContentPictures();
    void updateLeftPicture();
    void removeLeftPicture();
    void updateTopPicture();
    void removeTopPicture();
    void removeCornerPictures();
    void removeContentPictures();
    void refresh();
    void showOptionsDLG();
    void onClick();
    int save();
    int saveAs();
    void newFile();
    void openFile();
    void openFile(QString file);
    void print();
    void onCheckModified();
    void readSettings();
    void openRecentFile();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QActionGroup *m_modeActionGroup;
    QString m_lastOpenDir;
    QString m_lastSavedDir;
    BattleItem *m_cornerItem;
    PicturesContainer *m_container;
    bool m_refreshCornerOnClic;
    bool m_refreshContentOnClic;
    bool m_fileIsModified;
    QString m_currentFileName;
    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];
    QMenu *m_recentMenu;
    QStringList m_recentFilesList;
    QList<QAction *> recentFileActionList;

    QStringList getSelectedFiles(bool multipleSelection = true);
    QString strippedName(const QString &fullFileName);
    void updateRecentFileActions();
    void saveFile(QString fileName);
    void clearGame();
    void updateRecentFiles();
};

#endif // MAINWINDOW_H
