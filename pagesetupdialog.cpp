#include "pagesetupdialog.h"
#include "ui_pagesetupdialog.h"

PageSetupDialog::PageSetupDialog(RichTextProcess* editor1, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PageSetupDialog),
    editor(editor1)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose, true);
    this->setWindowTitle("页面设置");

    //初始化
    qDebug() << "初始化时" << editor->getCurrentPaper()->getPaperMargin().leftMargin;
    leftMargin = editor->getCurrentPaper()->getPaperMargin().leftMargin;
    rightMargin = editor->getCurrentPaper()->getPaperMargin().rightMargin;
    topMargin = editor->getCurrentPaper()->getPaperMargin().topMargin;
    bottomMargin = editor->getCurrentPaper()->getPaperMargin().bottomMargin;
    lineHeight = editor->getCurrentPaper()->getLineDistance();
    height = editor->getPaperHeight();
    width = editor->getPaperWidth();
    letterDistanceScale = editor->getCurrentPaper()->getLetterDistanceScale();

    ui->letterDistanceScale->setMaximum(4.00);
    ui->leftMargin->setMaximum(editor->getCurrentPaper()->getWidth()/2);
    ui->rightMargin->setMaximum(editor->getCurrentPaper()->getWidth()/2);
    ui->topMargin->setMaximum(editor->getCurrentPaper()->getHeight()/2);
    ui->bottomMargin->setMaximum(editor->getCurrentPaper()->getHeight()/2);
    ui->lineHeight->setMaximum(editor->getCurrentPaper()->getHeight()-
                               editor->getCurrentPaper()->getPaperMargin().topMargin-
                               editor->getCurrentPaper()->getPaperMargin().bottomMargin);

    ui->leftMargin->setValue(leftMargin);
    ui->rightMargin->setValue(rightMargin);
    ui->topMargin->setValue(topMargin);
    ui->bottomMargin->setValue(bottomMargin);
    ui->lineHeight->setValue(lineHeight);

    ui->letterDistanceScale->setValue(letterDistanceScale);
    qDebug() << letterDistanceScale;

    //设置单选按钮
    QButtonGroup* paperSize = new QButtonGroup(this);
    QButtonGroup* paperDir = new QButtonGroup(this);
    QButtonGroup* letterDistance = new QButtonGroup(this);
    QButtonGroup* lineDistance = new QButtonGroup(this);
    paperSize->addButton(ui->a4);
    paperSize->addButton(ui->a5);
    paperSize->addButton(ui->b5);
    paperDir->addButton(ui->vertical);
    paperDir->addButton(ui->horizontal);
    letterDistance->addButton(ui->letterDistanceToAll);
    lineDistance->addButton(ui->lineDistanceToAll);

    if(editor->getPaperHeight()==A4height&&editor->getPaperWidth()==A4width)
    {
        ui->a4->click();
        ui->vertical->click();
    }
    else if(editor->getPaperHeight()==A4width&&editor->getPaperWidth()==A4height)
    {
        ui->a4->click();
        ui->horizontal->click();
    }
    else if(editor->getPaperHeight()==B5height&&editor->getPaperWidth()==B5width)
    {
        ui->b5->click();
        ui->vertical->click();
    }
    else if(editor->getPaperHeight()==B5width&&editor->getPaperWidth()==B5height)
    {
        ui->b5->click();
        ui->horizontal->click();
    }
    else if(editor->getPaperHeight()==A5height&&editor->getPaperWidth()==A5width)
    {
        ui->a5->click();
        ui->vertical->click();
    }
    else if(editor->getPaperHeight()==A5width&&editor->getPaperWidth()==A5height)
    {
        ui->a5->click();
        ui->horizontal->click();
    }

    connect(ui->leftMargin, SIGNAL(valueChanged(double)), this, SLOT(receiveLeftMargin(double)));
    connect(ui->rightMargin, SIGNAL(valueChanged(double)), this, SLOT(receiveRightMargin(double)));
    connect(ui->topMargin, SIGNAL(valueChanged(double)), this, SLOT(receiveTopMargin(double)));
    connect(ui->bottomMargin, SIGNAL(valueChanged(double)), this, SLOT(receiveBottomMargin(double)));
    connect(ui->lineHeight, SIGNAL(valueChanged(double)), this, SLOT(receiveLineHeight(double)));
    connect(ui->letterDistanceScale, SIGNAL(valueChanged(double)), this, SLOT(receiveLetterDistance(double)));
}

PageSetupDialog::~PageSetupDialog()
{
    delete ui;
}

void PageSetupDialog::on_confirm_clicked()
{
    if(ui->a4->isChecked()&&ui->vertical->isChecked())
    {
        editor->setPaperHeight(A4height);
        editor->setPaperWidth(A4width);
        editor->resizePage();
    }
    else if(ui->a4->isChecked()&&ui->horizontal->isChecked())
    {
        editor->setPaperHeight(A4width);
        editor->setPaperWidth(A4height);
        editor->resizePage();
    }
    else if(ui->b5->isChecked()&&ui->vertical->isChecked())
    {
        editor->setPaperHeight(B5height);
        editor->setPaperWidth(B5width);
        editor->resizePage();
    }
    else if(ui->b5->isChecked()&&ui->horizontal->isChecked())
    {
        editor->setPaperHeight(B5width);
        editor->setPaperWidth(B5height);
        editor->resizePage();
    }
    else if(ui->a5->isChecked()&&ui->vertical->isChecked())
    {
        editor->setPaperHeight(A5height);
        editor->setPaperWidth(A5width);
        editor->resizePage();
    }
    else if(ui->a5->isChecked()&&ui->horizontal->isChecked())
    {
        editor->setPaperHeight(A5width);
        editor->setPaperWidth(A5height);
        editor->resizePage();
    }
    if(ui->marginToAll->isChecked())
    {
        editor->setPaperMarginToAll(leftMargin, rightMargin, topMargin, bottomMargin);
    }
    else
    {
        editor->getCurrentPaper()->setLeftMargin(leftMargin);
        editor->getCurrentPaper()->setRightMargin(rightMargin);
        editor->getCurrentPaper()->setTopMargin(topMargin);
        editor->getCurrentPaper()->setBottomMargin(bottomMargin);
    }

    editor->setlineDistance(lineHeight, ui->lineDistanceToAll->isChecked());
    editor->setLetterDistanceScale(letterDistanceScale, ui->letterDistanceToAll->isChecked());
    this->close();
}

void PageSetupDialog::on_cancel_clicked()
{
    this->close();
}

void PageSetupDialog::receiveBottomMargin(double bottom)
{
    bottomMargin = bottom;
}

void PageSetupDialog::receiveLeftMargin(double left)
{
    leftMargin = left;
}

void PageSetupDialog::receiveRightMargin(double right)
{
    rightMargin = right;
}

void PageSetupDialog::receiveTopMargin(double top)
{
    topMargin = top;
}

void PageSetupDialog::receiveLineHeight(double line)
{
    lineHeight = line;
}

void PageSetupDialog::receiveLetterDistance(double l)
{
    letterDistanceScale = l;
}

void PageSetupDialog::on_pushButton_2_clicked()
{
    leftMargin = 30.00;
    rightMargin = 30.00;
    topMargin = 30.00;
    bottomMargin = 30.00;
    lineHeight = 3.5;
    letterDistanceScale = 1;

    ui->leftMargin->setValue(leftMargin);
    ui->rightMargin->setValue(rightMargin);
    ui->topMargin->setValue(topMargin);
    ui->bottomMargin->setValue(bottomMargin);
    ui->lineHeight->setValue(lineHeight);
    ui->letterDistanceScale->setValue(letterDistanceScale);

    ui->a4->click();
    ui->vertical->click();
}
