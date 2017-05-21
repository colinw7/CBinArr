#include <CStrUtil.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>

static void process_file(const std::string &fileName);

static bool encode_size    = false;
static bool lower          = false;
static bool cclass         = false;
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
      else if (strcmp(&argv[i][1], "chars_per_line") == 0) {
        ++i;

        if (i < argc)
          chars_per_line = atoi(argv[i]);
      }
      else
        std::cerr << "Invalid option \'" << argv[i] << "\'" << std::endl;
    }
    else
      files.push_back(argv[i]);
  }

  uint num_files = files.size();

  if (num_files == 0) {
    std::cerr << "Usage: CBinArr [-encode_size] [-lower] [-class] [-chars_per_line <n>] "
                 "<file> ..." << std::endl;
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

  std::cout << "#ifndef " << fileName2 << "_pixmap_H" << std::endl;
  std::cout << "#define " << fileName2 << "_pixmap_H" << std::endl;
  std::cout << std::endl;

  if (cclass) {
    std::cout << "#include <CQPixmapCache.h>" << std::endl;
    std::cout << std::endl;

    std::cout << "class " << fileName2 << "_pixmap {" << std::endl;
  }

  std::string indent, indent1;

  if (! cclass) {
    if (! lower)
      std::cout << "#define " << fileName2 << "_DATA_LEN  " << file_size << std::endl;
    else
      std::cout << "#define " << fileName2 << "_data_len  " << file_size << std::endl;

    std::cout << std::endl;

    if (! lower)
      std::cout << "uchar " << fileName1 << "_data[" << fileName2 <<
                   "_DATA_LEN" << (encode_size ? " + 16" : "") << "] = {" << std::endl;
    else
      std::cout << "uchar " << fileName1 << "_data[" << fileName2 <<
                   "_data_len" << (encode_size ? " + 16" : "") << "] = {" << std::endl;
  }
  else {
    indent  = "  ";
    indent1 = "    ";

    std::cout << " private:" << std::endl;
    std::cout << "  uchar data_[" << file_size << "] = {" << std::endl << indent1;
  }

  if (encode_size) {
    char length_string[17];

    sprintf(length_string, "%d", file_size);

    for (int i = 0; i < 16; i++)
      CStrUtil::printf("0x%02x,", length_string[i] & 0xFF);

    std::cout << std::endl;
  }

  int pos = 0;

  int c = fgetc(fp);

  while (c != EOF) {
    CStrUtil::printf("0x%02x,", c & 0xFF);

    ++pos;

    if (pos == chars_per_line) {
      pos = 0;

      std::cout << std::endl << indent1;
    }

    c = fgetc(fp);
  }

  fclose(fp);

  if (pos > 0)
    std::cout << std::endl;

  std::cout << indent << "};" << std::endl;

  if (cclass) {
    std::cout << std::endl;
    std::cout << " public:" << std::endl;
    std::cout << "  " << fileName2 << "_pixmap() {" << std::endl;
    std::cout << "    CQPixmapCache::instance()->addData(\"" << fileName2 << "\", data_, " <<
                 file_size << ");" << std::endl;
    std::cout << "  }" << std::endl;
  }

  if (cclass) {
    std::cout << "};" << std::endl;
    std::cout << std::endl;
    std::cout << "static " << fileName2 << "_pixmap s_" << fileName2 << "_pixmap;" << std::endl;
  }

  std::cout << std::endl;
  std::cout << "#endif" << std::endl;
}
