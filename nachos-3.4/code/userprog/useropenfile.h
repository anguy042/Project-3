#ifndef UserOpenFile_H
#define UserOpenFile_H

#include "list.h"
#include "sysopenfile.h"


class UserOpenFile {

    public:
        UserOpenFile(SysOpenFile *sysOpenFile, int fileID, char *fileName);
        ~UserOpenFile();
        SysOpenFile *sysOpenFile;
        int offset;
        char *fileName;
        //TODO: Index into the global SysOpenFile
};

#endif // UserOpenFile_H