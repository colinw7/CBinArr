#include <CStrUtil.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>

static void process_file(const std::string &fileName);

static bool encode_size    = false;
static bool lower          = false;
static bool cclass         = false;
static bool qsvg           = false;
static bool icon           = false;
static int  chars_per_line = 16;

int
main(int argc, char **argv)
{
  std::vector<std::string> files;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if      (strcmp(&argv[i][1], "encode_size") == 0)
        encode_size = true;
      else if (strcmp(&argv[i][1], "lower") == 0)
        lower = true;
      else if (strcmp(&argv[i][1], "class") == 0)
        cclass = true;
      else if (strcmp(&argv[i][1], "icon") == 0)
        icon = true;
      else if (strcmp(&argv[i][1], "qsvg") == 0)
        qsvg = true;
      else if (strcmp(&argv[i][1], "chars_per_line") == 0) {
        ++i;

        if (i < argc)
          chars_per_line = atoi(argv[i]);
      }
      else
        std::cerr << "Invalid option \'" << argv[i] << "\'\n";
    }
    else
      files.push_back(argv[i]);
  }

  uint num_files = files.size();

  if (num_files == 0) {
    std::cerr << "Usage: CBinArr [-encode_size] [-lower] [-class] [-chars_per_line <n>] "
                 "<file> ...\n";
    exit(1);
  }

  for (uint i = 0; i < num_files; i++)
    process_file(files[i]);

  return 0;
}

static void
process_file(const std::string &fileName)
{
  FILE *fp = fopen(fileName.c_str(), "rb");

  if (fp == NULL)
    return;

  fseek(fp, 0, SEEK_END);

  int file_size = ftell(fp);

  fseek(fp, 0, SEEK_SET);

  //---

  std::string fileName1 = fileName;

  uint len1 = fileName1.size();

  for (uint i = 0; i < len1; ++i) {
    if (fileName1[i] == '.') {
      fileName1 = fileName1.substr(0, i);
      break;
    }
  }

  //---

  std::string fileName2 = fileName1;

  if (! lower) {
    for (uint i = 0; i < fileName2.size(); ++i)
      if (islower(fileName2[i]))
        fileName2[i] = toupper(fileName2[i]);
  }

  //---

  if (icon) {
    std::cout << "#ifndef CIcon_" << fileName2 << "_H\n";
    std::cout << "#define CIcon_" << fileName2 << "_H\n";
    std::cout << "\n";
  }
  else if (qsvg) {
    std::cout << "#ifndef " << fileName2 << "_SVG_H\n";
    std::cout << "#define " << fileName2 << "_SVG_H\n";
    std::cout << "\n";
  }
  else {
    std::cout << "#ifndef " << fileName2 << "_pixmap_H\n";
    std::cout << "#define " << fileName2 << "_pixmap_H\n";
    std::cout << "\n";
  }

  if      (cclass) {
    std::cout << "#include <CQPixmapCache.h>\n";
    std::cout << "\n";

    std::cout << "class " << fileName2 << "_pixmap {\n";
  }
  else if (qsvg) {
    std::cout << "#include <QSvgRenderer>\n";
    std::cout << "#include <QPainter>\n";
    std::cout << "#include <QImage>\n";
    std::cout << "\n";

    std::cout << "class " << fileName2 << "_SVG {\n";
  }
  else if (icon) {
    std::cout << "class CIcon_" << fileName2 << " {\n";
  }

  std::string indent, indent1;

  if      (cclass) {
    indent  = "  ";
    indent1 = "    ";

    std::cout << " private:\n";
    std::cout << "  uchar data_[" << file_size << "] = {\n" << indent1;
  }
  else if (qsvg) {
    indent  = "  ";
    indent1 = "    ";

    std::cout << " private:\n";
    std::cout << "  uchar data_[" << file_size << "] = {\n" << indent1;
  }
  else if (icon) {
    indent  = "  ";
    indent1 = "    ";

    std::cout << " private:\n";
    std::cout << "  uchar data_[" << file_size << "] = {\n" << indent1;
  }
  else {
    if (! lower)
      std::cout << "#define " << fileName2 << "_DATA_LEN  " << file_size << "\n";
    else
      std::cout << "#define " << fileName2 << "_data_len  " << file_size << "\n";

    std::cout << "\n";

    if (! lower)
      std::cout << "uchar " << fileName1 << "_data[" << fileName2 <<
                   "_DATA_LEN" << (encode_size ? " + 16" : "") << "] = {\n";
    else
      std::cout << "uchar " << fileName1 << "_data[" << fileName2 <<
                   "_data_len" << (encode_size ? " + 16" : "") << "] = {\n";
  }

  if (encode_size) {
    char length_string[17];

    sprintf(length_string, "%d", file_size);

    for (int i = 0; i < 16; i++)
      CStrUtil::printf("0x%02x,", length_string[i] & 0xFF);

    std::cout << "\n";
  }

  int pos = 0;

  int c = fgetc(fp);

  while (c != EOF) {
    CStrUtil::printf("0x%02x,", c & 0xFF);

    ++pos;

    if (pos == chars_per_line) {
      pos = 0;

      std::cout << "\n" << indent1;
    }

    c = fgetc(fp);
  }

  fclose(fp);

  if (pos > 0)
    std::cout << "\n";

  std::cout << "  };\n";

  if      (cclass) {
    std::cout << "\n";
    std::cout << " public:\n";
    std::cout << "  " << fileName2 << "_pixmap() {\n";
    std::cout << "    CQPixmapCache::instance()->addData(\"" << fileName2 << "\", data_, " <<
                 file_size << ");\n";
    std::cout << "  }\n";
  }
  else if (qsvg) {
    std::cout << "\n";
    std::cout << " public:\n";
    std::cout << "  " << fileName2 << "_SVG() { }\n";
    std::cout << "\n";
    std::cout << "  QImage image(int w, int h) {\n";
    std::cout << "    if (image_.isNull() || image_.width() == w || image_.height() == h) {\n";
    std::cout << "      QSvgRenderer renderer(QByteArray((char *) data_, " << file_size << "));\n";
    std::cout << "\n";
    std::cout << "      image_ = QImage(w, h, QImage::Format_ARGB32);\n";
    std::cout << "      image_.fill(0);\n";
    std::cout << "\n";
    std::cout << "      QPainter painter(&image_);\n";
    std::cout << "      renderer.render(&painter);\n";
    std::cout << "    }\n";
    std::cout << "\n";
    std::cout << "    return image_;\n";
    std::cout << "  }\n";
    std::cout << "\n";
    std::cout << " private:\n";
    std::cout << "  QImage image_;\n";
    std::cout << "};\n";
  }
  else if (icon) {
    std::cout << "\n";
    std::cout << " public:\n";
    std::cout << "  static CIcon_" << fileName2 << " *instance() {\n";
    std::cout << "    static CIcon_" << fileName2 << " *inst;\n";
    std::cout << "\n";
    std::cout << "    if (! inst)\n";
    std::cout << "      inst = new CIcon_" << fileName2 << ";\n";
    std::cout << "\n";
    std::cout << "    return inst;\n";
    std::cout << "  }\n";
    std::cout << "\n";

    std::cout << "  static QIcon icon() {\n";
    std::cout << "    return instance()->getIcon();\n";
    std::cout << "  }\n";
    std::cout << "\n";

    std::cout << " private:\n";
    std::cout << "  CIcon_" << fileName2 << "() { }\n";
    std::cout << "\n";

    std::cout << "  QIcon getIcon() const {\n";
    std::cout << "    if (! pixmap_) {\n";
    std::cout << "      auto th = const_cast<CIcon_" << fileName2 << " *>(this);\n";
    std::cout << "\n";
    std::cout << "      th->pixmap_ = new QPixmap;\n";
    std::cout << "\n";
    std::cout << "      th->pixmap_->loadFromData(data_, 45191);\n";
    std::cout << "    }\n";
    std::cout << "\n";
    std::cout << "    return QIcon(*pixmap_);\n";
    std::cout << "  }\n";
    std::cout << "\n";

    std::cout << " private:\n";
    std::cout << "  QPixmap *pixmap_ { nullptr };\n";
    std::cout << "};\n";
  }

  if      (cclass) {
    std::cout << "};\n";
    std::cout << "\n";
    std::cout << "static " << fileName2 << "_pixmap s_" << fileName2 << "_pixmap;\n";
  }
  else if (qsvg) {
    std::cout << "\n";
    std::cout << "static " << fileName2 << "_SVG s_" << fileName2 << "_SVG;\n";
  }

  std::cout << "\n";
  std::cout << "#endif\n";
}
