#ifndef OPTIONSDLG_H
#define OPTIONSDLG_H

#include <QDialog>

namespace Ui {
class OptionsDLG;
}

class OptionsDLG : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDLG(bool refreshCorner, bool refreshContent, QWidget *parent = nullptr);
    ~OptionsDLG();

    bool refreshCorner();
    bool refreshContent();

private:
    Ui::OptionsDLG *ui;
};

#endif // OPTIONSDLG_H
