#ifndef BITMAPGLUE_HPP
#define BITMAPGLUE_HPP

#include "helpwindow.hpp"
#include <QMainWindow>
#include <QImage>
#include <QColor>
#include <qmath.h>
#include <QRgb>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QVector>
#include <QModelIndexList>
#include <QCloseEvent>
#include <QTextStream>

namespace Ui {
  class BitmapGlue;
}

class BitmapGlue : public QMainWindow
{
  Q_OBJECT

public:
  explicit BitmapGlue(QWidget *parent = 0);
  QImage FinalSlmImage(QString filePath);
  QImage GluedSlmImage();

  void closeEvent(QCloseEvent *event);

  ~BitmapGlue();

private slots:
  void on_Cancel_clicked();

  void on_BmpPick_clicked();

  void on_SaveBmp_clicked();

  void on_SLMSize_currentIndexChanged(int index);

  void on_TypePick_currentIndexChanged(int index);

  void on_BmpRes_currentIndexChanged(int index);

  void on_pushButton_clicked();

  void on_actionO_programie_triggered();

  void on_actionPomoc_triggered();

private:
  Ui::BitmapGlue *ui;
  QImage SlmImageBmp;
  QString bmpPath,bmpsPath,inputBmpPath,dirPath;
  int BmpWidth, BmpHeight;
  int ScaleW,ScaleH;
  int FinalW, FinalH;
  int OffsetW, OffsetH;
  int BmpRes;
  QVector<int> *BmpResolutions;
};

#endif // BITMAPGLUE_HPP
