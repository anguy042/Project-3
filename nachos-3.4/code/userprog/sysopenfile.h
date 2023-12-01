#ifndef SysOpenFile_H
#define SysOpenFile_H

#include "list.h"
#include "openfile.h"

class SysOpenFile {

    public:
        SysOpenFile(OpenFile *openFile, int fileID, char *fileName);
        ~SysOpenFile();
        OpenFile *openFile;
        int fileID;
        char *fileName;
        int userOpens;
};

#endif // SysOpenFile_H