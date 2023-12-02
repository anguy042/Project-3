#ifndef SysOpenFile_H
#define SysOpenFile_H

#include "list.h"
#include "openfile.h"

class SysOpenFile {
    public:
        SysOpenFile(OpenFile *openFileInput, int id,char *fileNameInput);
        ~SysOpenFile();
        OpenFile *openFile;
        int fileID;
        char *fileName;
        int userOpens;
};

#endif // SysOpenFile_H