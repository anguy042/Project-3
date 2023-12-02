#include "sysopenfile.h"

SysOpenFile::SysOpenFile(OpenFile *openFileInput, int id,char *fileNameInput) {
    openFile = openFileInput;
    fileID = id;
    fileName = fileNameInput;
}