#ifndef __FILEINFO__H__
#define __FILEINFO__H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include <string>
#include <array>

#define MAX_FILENAME_LEN 128
#define SEP_CHAR '\0'
#define SEP_LEN 2

inline size_t numToStr(char *tmp,size_t n);

void encode_fileinfo(std::array<char,MAX_FILENAME_LEN>& a,const std::string& filename);
void decode_fileinfo(std::string& filename,const std::array<char,MAX_FILENAME_LEN>& a);

#endif
