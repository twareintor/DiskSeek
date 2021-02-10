


#include <iostream>
#include <fstream.h>    // to be removed 




int CopyFile(const char* sz_Src, const char* sz_Des)
{
    //////// see: "http://www.programmersheaven.com/download/13834/Download.aspx"
    ifstream ifs(sz_Src,ios::in |ios::binary);
    ofstream ofs(sz_Des,ios::out|ios::binary);
    ofs<<ifs.rdbuf();
    ifs.close();
    ofs.close();
    return 1;
    
}
