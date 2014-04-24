
#ifndef EXCEL_H
#define EXCEL_H

// for Visual Studio Warnings 
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <cstring>

#define MAXLEN 1024

class Excel
{
public:
  Excel();
  ~Excel();

public:
  bool Open(const char* filename);
  void Close();
  bool IsOpen();

  void Begin();
  void End();
  void Write(const int row, const int col, const char* value);
  void Write(const int row, const int col, const double value);

  const char* LastError() const;

private:
  inline void putcc(FILE* file, const int value);

private:
  FILE* file;
  char error[MAXLEN];
};

#endif
