#include "helpwindow.hpp"
#include "ui_helpwindow.h"

HelpWindow::HelpWindow(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::HelpWindow)
{
  ui->setupUi(this);
  QPixmap pixmap(":/graphics/gluegun.png");
  pixmap=pixmap.scaled(QSize(100,100));
  ui->icon->setPixmap(pixmap);
  setWindowTitle("Pomoc");
}

HelpWindow::~HelpWindow()
{
  delete ui;
}

void HelpWindow::on_OkButton_clicked()
{
    this->close();
}
