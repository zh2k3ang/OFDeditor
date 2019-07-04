#include "insertimagedialog.h"
#include "ui_insertimagedialog.h"

InsertImageDialog::InsertImageDialog(RichTextProcess* editor1, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InsertImageDialog),
    editor(editor1)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose, true);
    this->setWindowTitle("插入图片");
    ui->imageHeight->setMaximum(1000);
    ui->imageWidth->setMaximum(1000);
    connect(ui->imageHeight, SIGNAL(valueChanged(double)), this, SLOT(receiveHeight(double)));
    connect(ui->imageWidth, SIGNAL(valueChanged(double)), this, SLOT(receiveWidth(double)));
}

InsertImageDialog::~InsertImageDialog()
{
    delete ui;
}

void InsertImageDialog::on_selectImage_clicked()
{
    //获取图片路径
    QString imagePath = QFileDialog::getOpenFileName(this, tr("选择图片路径"),
                                     ".", tr("JPEG (*.jpg *jpeg)\n"
                                       "Bitmap Files (*.bmp)\n"
                                       "GIF (*.gif)\n"
                                       "PNG (*.png)\n"));
    path = imagePath;
    ui->showPath->setText(imagePath);
    QImage image(imagePath);

    //将图片缩放到合适比例
    double scale = (double)image.height()/(double)image.width();
    width = 400;
    height = scale*width;

    ui->imageHeight->setValue(height);
    ui->imageWidth->setValue(width);
}

void InsertImageDialog::on_no_clicked()
{
    this->close();
}

void InsertImageDialog::on_yes_clicked()
{
    editor->insertImage(path, height, width);
    this->close();
}

void InsertImageDialog::receiveHeight(double imageheight)
{
    height = imageheight;
}

void InsertImageDialog::receiveWidth(double imagewidth)
{
    width = imagewidth;
}

void InsertImageDialog::on_resetScale_clicked()
{
    QImage image(path);
    //将图片缩放到合适比例
    double scale = (double)image.height()/(double)image.width();
    height = scale*width;

    ui->imageHeight->setValue(height);
    ui->imageWidth->setValue(width);
}
