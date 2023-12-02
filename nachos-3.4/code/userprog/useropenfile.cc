#include "useropenfile.h"

UserOpenFile::UserOpenFile(SysOpenFile *sysOpenFileInput, int offsetInput, char *fileNameInput) {
    sysOpenFile = sysOpenFileInput;
    offset = offsetInput;
    fileName = fileNameInput;
}