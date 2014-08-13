#include "bitmapglue.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  BitmapGlue w;
  w.show();

  return a.exec();
}
