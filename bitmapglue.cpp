#include "bitmapglue.hpp"
#include "ui_bitmapglue.h"
#include <QDesktopWidget>
#include <QDebug>

BitmapGlue::BitmapGlue(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::BitmapGlue)
{
  ui->setupUi(this);
  showMaximized();
  setWindowTitle("BitmapGlue");
  setWindowIcon(QIcon(":/icons/gluegun.ico"));
  ui->BmpName->setText("Nie wybrano bitmapy");
  ui->SlmImage->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  QSize InputSize(256,256);
  BmpRes=128;
  ui->InputBitmap->setFixedSize(InputSize);
  BmpResolutions=new QVector<int>();

  dirPath=QApplication::applicationDirPath();
  dirPath.chop(4);

  ui->InputBitmaps->setSelectionMode(QAbstractItemView::SingleSelection );
  ui->InputBitmaps->setDragDropMode(QAbstractItemView::InternalMove);

  if(ui->SLMSize->currentIndex()==0) {
      FinalW=1920;
      FinalH=1080;
    }
  else if(ui->SLMSize->currentIndex()==1) {
      FinalW=4096;
      FinalH=2048;
    }

  QRegExp wzor("(\\d{3,4})x\\d{3,4}\\s\\px");
  QFile resolutions(QApplication::applicationDirPath()+"/resolutions.cnf");
  if(resolutions.open(QIODevice::ReadOnly)) {
      QTextStream in(&resolutions);
      QString line;
      while(!in.atEnd()) {
          line=in.readLine();
          wzor.indexIn(line);
          BmpResolutions->push_back(wzor.cap(1).toInt());
          ui->BmpRes->addItem(line);
        }
      resolutions.close();
    }
  else
    QMessageBox::information(this,"Błąd otwierania pliku konfiguracyjnego","Wystąpił błąd w trakcie otwierania pliku \"resolutions.cnf\"",
                             QMessageBox::Ok);
}

BitmapGlue::~BitmapGlue()
{
  delete ui;
}

void BitmapGlue::on_Cancel_clicked()
{
  this->close();
}

void BitmapGlue::closeEvent(QCloseEvent *event)
{
  QMessageBox CloseQuestion("Zamykanie...","Czy jesteś pewien, że chcesz wyjść z programu?",QMessageBox::NoIcon,QMessageBox::Close,
                            QMessageBox::Cancel,QMessageBox::NoButton,this);
  CloseQuestion.button(QMessageBox::Close)->setText("Zamknij");
  CloseQuestion.button(QMessageBox::Cancel)->setText("Anuluj");
  if(CloseQuestion.exec()==QMessageBox::Close)
    event->accept();
  else
    event->ignore();
}

void BitmapGlue::on_BmpPick_clicked()
{
  if(ui->TypePick->currentIndex()==0) {
      //Dobranie się do ścieżki przez QFile i do nazwy pliku przez QFileInfo
      QFileDialog dialog(this);
      dialog.setDirectory(dirPath);
      dialog.setFileMode(QFileDialog::ExistingFile);
      dialog.setNameFilter("Bitmapy (*.bmp);;Wszystkie pliki (*.*)");
      if(dialog.exec()) {
          QFile InputBmpFile(dialog.selectedFiles().at(0));
          QFileInfo InputBmpInfo(InputBmpFile.fileName());
          //Sprawdzanie, czy wczytywany plik jest bmp
          QRegExp wzor("(.*)\\.(bmp)");
          wzor.indexIn(InputBmpInfo.fileName());
          if(QString::compare("bmp",wzor.cap(2))==0) {
              ui->BmpName->setText(InputBmpInfo.fileName());
              inputBmpPath=InputBmpFile.fileName();
            }
          else
            QMessageBox::warning(this,"Błędny plik...",
              "Wybrano plik o błędnym rozszerzeniu lub nie wybrano pliku. Program obsługuje tylko pliki o rozszerzeniu \".bmp\"",QMessageBox::Ok);
          this->activateWindow();
        }
    }
  else if(ui->TypePick->currentIndex()==1) {
      QStringList bmpList;
      QFileDialog dialog(this);
      dialog.setDirectory(dirPath);
      dialog.setFileMode(QFileDialog::ExistingFiles);
      dialog.setNameFilter("Bitmapy (*.bmp);;Wszystkie pliki (*.*)");
      QString bitmaps;
      if(dialog.exec()) {
          bmpList=dialog.selectedFiles();
          if(bmpList.count()==1)
            bitmaps=" bitmapę";
          else if(bmpList.count()>1 && bmpList.count()<5)
            bitmaps=" bitmapy";
          else if(bmpList.count()>5)
            bitmaps=" bitmap";
          ui->BmpName->setText("Wybrano "+QString::number(bmpList.count())+bitmaps);
        }
      if(bmpList.count()!=qFloor(FinalH/BmpRes)*qFloor(FinalW/BmpRes))
        QMessageBox::warning(this,"Błędna ilość plików...",
                QString("Wybrano złą ilość plików.\nKonieczne jest załadowanie %1 bitmap przy obecnych ustawieniach.").
                                 arg(qFloor(FinalH/BmpRes)*qFloor(FinalW/BmpRes)),QMessageBox::Ok);
      else if(bmpList.size() != 0) {
          QList<QFile*> fileList;
          QStringList fileNames;
          QFileInfo* tmp;
          for(int i=0;i<bmpList.size();++i) {
              fileList.append(new QFile(bmpList.at(i)));
              tmp=new QFileInfo(fileList.at(i)->fileName());
              fileNames.append(tmp->fileName());
            }
          ui->InputBitmaps->clear();
          ui->InputBitmaps->addItems(fileNames);
          bmpsPath=bmpList.at(0);
          int fileLnght = fileNames.at(0).size();
          bmpsPath.chop(fileLnght);
          SlmImageBmp=GluedSlmImage();
        }
      else
        QMessageBox::warning(this,"Błędny plik...",
          "Wybrano pliki o błędnym rozszerzeniu lub nie wybrano plików. Program obsługuje tylko pliki o rozszerzeniu \".bmp\"",QMessageBox::Ok);
    }
}

//Funkcja do klejenia powielonych bitmap
QImage BitmapGlue::FinalSlmImage(QString filePath) {

  bmpPath=filePath;

  //Deklaracje zmiennych
  QImage InputBmp(filePath);
  QRgb black=qRgb(0,0,0);

  ui->InputBitmap->setPixmap(QPixmap::fromImage(InputBmp));

  //Obliczanie parametrów
  BmpHeight=InputBmp.height();
  BmpWidth=InputBmp.width();

  ScaleW=qFloor(FinalW/BmpWidth);
  ScaleH=qFloor(FinalH/BmpHeight);
  OffsetW=(FinalW-ScaleW*BmpWidth)/2;
  OffsetH=(FinalH-ScaleH*BmpHeight)/2;

  //Pomocniczy obraz sklejonych obrazów, bez offsetu
  QImage gluedImage(BmpWidth*ScaleW,BmpHeight*ScaleH,QImage::Format_RGB888);
  gluedImage.fill(black);
  for(int k=0;k<ScaleH;++k)
    for(int l=0;l<ScaleW;++l)
      for(int i=0;i<BmpHeight;++i)
        for(int j=0;j<BmpWidth;++j)
          gluedImage.setPixel(l*BmpWidth+j,k*BmpHeight+i,InputBmp.pixel(j,i));

  //Obraz sklejonych bitmap z dodaniem offsetu
  QImage SlmImage(FinalW,FinalH,QImage::Format_RGB32);
  SlmImage.fill(black);
  for(int i=0;i<gluedImage.height();++i)
    for(int j=0;j<gluedImage.width();++j)
      SlmImage.setPixel(OffsetW+j,OffsetH+i,gluedImage.pixel(j,i));

  return SlmImage;
}

//Funkcja do klejenia wielu bitmap
QImage BitmapGlue::GluedSlmImage() {

  //Obliczanie parametrów
  ScaleW=qFloor(FinalW/BmpRes);
  ScaleH=qFloor(FinalH/BmpRes);
  OffsetW=(FinalW-ScaleW*BmpRes)/2;
  OffsetH=(FinalH-ScaleH*BmpRes)/2;

  //Wczytanie listy bitmap wejściowych
  QList<QListWidgetItem*> finalList;
  finalList=ui->InputBitmaps->findItems("*",Qt::MatchWrap|Qt::MatchWildcard);
  QVector<QImage*> imageList;
  QVector<QFile*> fileList;
  for(int i=0;i<finalList.size();++i) {
      fileList.append(new QFile(bmpsPath+finalList.at(i)->text()));
      imageList.append(new QImage(fileList.at(i)->fileName()));
    }
  QRgb black=qRgb(0,0,0);

  //Sprawdzenie wymiarów wczytanych bitmap
  BmpWidth=imageList.at(0)->width();
  BmpHeight=imageList.at(0)->height();

  QString info=QString("Wczytano bitmapy o rozmiarze innym, niż zadeklarowany.\nRozmiar wczytanych bitmap: %1x%2 px\nWczytaj bitmapy o prawidłowym rozmiarze.").arg(BmpWidth).arg(BmpHeight);

  if(BmpWidth!=BmpRes) {
      QMessageBox::warning(this,"Błędny rozmiar wczytanych bitmap",info,QMessageBox::Ok);
      return *imageList.at(0);
    }
  else {

      //Pomocniczy obraz sklejonych bitmap, bez offsetu
      QImage gluedImage(BmpWidth*ScaleW,BmpHeight*ScaleH,QImage::Format_RGB888);
      gluedImage.fill(black);
      for(int k=0;k<ScaleH;++k)
        for(int l=0;l<ScaleW;++l)
          for(int i=0;i<BmpHeight;++i)
            for(int j=0;j<BmpWidth;++j)
              gluedImage.setPixel(l*BmpWidth+j,k*BmpHeight+i,imageList.at(k*ScaleW+l)->pixel(j,i));

      //Obraz sklejonych bitmap z dodaniem offsetu
      QImage SlmImage(FinalW,FinalH,QImage::Format_RGB32);
      SlmImage.fill(black);
      for(int i=0;i<gluedImage.height();++i)
        for(int j=0;j<gluedImage.width();++j)
          SlmImage.setPixel(OffsetW+j,OffsetH+i,gluedImage.pixel(j,i));

      return SlmImage;
    }
}

//Akcja po naciśnięciu "Zapisz bitmapę"
void BitmapGlue::on_SaveBmp_clicked()
{
  //Zapis, jeśli wcześniej wygenerowano bitmapę wyjściową.
  if(!SlmImageBmp.isNull()) {
      SlmImageBmp=SlmImageBmp.convertToFormat(QImage::Format_Indexed8);
      QString SavedBmpPath=QFileDialog::getSaveFileName(this,"Zapisywanie","","Bitmapy (*.bmp)");
      SlmImageBmp.save(SavedBmpPath,0,100);
    }
  else
    QMessageBox::warning(this,"Błąd zapisu","Należy najpierw wygenerować bitmapę wyjściową.",QMessageBox::Ok);
}

//Akcja po zmianie wymiarów bitmapy wynikowej
void BitmapGlue::on_SLMSize_currentIndexChanged(int index)
{
  QString resol;
  if(index==0) {
      resol=QString("1920x1080");
      FinalW=1920;
      FinalH=1080;
    }
  else if(index==1) {
      resol=QString("4096x2048");
      FinalW=4096;
      FinalH=2048;
    }

  //Sprawdzenie, czy konieczne jest przeliczenie bitmapy wyjściowej
  if(!SlmImageBmp.isNull())
    if(QMessageBox::question(this,"Przeliczenie bitmapy","Czy przeliczyć nową bitmapę w nowej rozdzielczości: "+resol+" ?",QMessageBox::Yes,QMessageBox::No)==QMessageBox::Yes)
      ui->SlmImage->setPixmap(QPixmap::fromImage(FinalSlmImage(bmpPath)));
}

//Akcja po wyborze typu klejenia bitmap
void BitmapGlue::on_TypePick_currentIndexChanged(int index)
{
  SlmImageBmp.fill(qRgb(0,0,0));
  ui->SlmImage->setPixmap(QPixmap::fromImage(SlmImageBmp));
  ui->TypeIndicator->setCurrentIndex(index);
  ui->TypeIndicator2->setCurrentIndex(index);
}

//Akcja po zmianie wymiarów bitmap wejściowych
void BitmapGlue::on_BmpRes_currentIndexChanged(int index) {
  BmpRes=BmpResolutions->at(index);
  if(BmpRes>FinalH) {
      if(QMessageBox::warning(this,"Błąd rozmiarów","Wybrany rozmiar wczytywanych bitmap jest za duży!\nNastąpi zmiana rozmiaru wczytywanych bitmap do maksymalnego dopuszczalnego przy obecnym rozmiarze bitmapy wynikowej",QMessageBox::Ok,QMessageBox::Abort)==QMessageBox::Ok)
        for(int i=0;i<BmpResolutions->size()-1;++i) {
            ui->BmpRes->setCurrentIndex(i);
            if(BmpResolutions->at(i+1)>FinalH)
                break;
          }
    }
}

//Akcja po naciśnięciu "Generuj bitmapę"
void BitmapGlue::on_pushButton_clicked()
{
  if(ui->TypePick->currentIndex()==0)
    SlmImageBmp=FinalSlmImage(inputBmpPath);
  if(ui->TypePick->currentIndex()==1)
    SlmImageBmp=GluedSlmImage();
  ui->SlmImage->setPixmap(QPixmap::fromImage(SlmImageBmp));
  ui->SlmImage->adjustSize();
}

void BitmapGlue::on_actionO_programie_triggered()
{
  QMessageBox msg(this);
  msg.setWindowTitle("O programie");
  msg.setTextFormat(Qt::RichText);
  msg.setText("Program napisany dla Zakładu II Wydziału Fizyki Politechniki Warszawskiej<br />i przeznaczony do jego wyłącznego użytku.<br /><br />Autor: inż. Robert Chudek<br /><br />Kontakt<br />E-mail: <a href=\"mailto:ro.chudek@gmail.com?Subject=BitmapGlue%20problem\" target=\"_top\">ro.chudek@gmail.com");
  msg.setStandardButtons(QMessageBox::Ok);
  msg.setIcon(QMessageBox::Information);
  msg.exec();
}

void BitmapGlue::on_actionPomoc_triggered()
{
  HelpWindow *wnd=new HelpWindow(this);
  wnd->show();
}
