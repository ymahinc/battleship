#include "optionsdlg.h"
#include "ui_optionsdlg.h"

OptionsDLG::OptionsDLG(bool refreshCorner, bool refreshContent, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDLG)
{
    ui->setupUi(this);

    ui->refreshCornerOnClickCheckBox->setChecked(refreshCorner);
    ui->refreshContentOnClickCheckBox->setChecked(refreshContent);
}

OptionsDLG::~OptionsDLG()
{
    delete ui;
}

bool OptionsDLG::refreshCorner(){
    return ui->refreshCornerOnClickCheckBox->isChecked();
}

bool OptionsDLG::refreshContent(){
    return ui->refreshContentOnClickCheckBox->isChecked();
}
