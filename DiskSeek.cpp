/***********************************************************************************************\
*                                                                                               *
*               BEGIN OF FILE "DiskSeek.cpp" ver. 1.0.0-A01/11.09.26 10:00:00                   *
*                                                                                               *
\***********************************************************************************************/

/***********************************************************************************************\
*      Release notes: Die vorherige VErsion (100-A00) hat +überhaupt nicht funktioniert         *
*        Die Funktion open() hat zu langsam reagiert und nix war geschrieben in die             *
*                        ausgabe Dateien                                                        *
\***********************************************************************************************/


#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <string.h>
#include <fstream>    // to be removed 
#define SEC_SIZE        0x1000000      //
#define JPG_SIZE        0x10000      //
#define MAX_JPEG        0x400000     // allocate memory for an image...
#define OK              1
#define ERR            -1

using namespace std;


//////////////////////  PROTOTYPES ////////////////////////////////////////

int   Proceed(); // it does what its name says
short ReadSect(
               const char     *_dsk,    // disk to access
               char           *_buff,         // buffer where sector will be stored
               LONG            _nsect,   // sector number, starting with 0
               PLONG           _offset // sector offset
               );
int SaveJPGFile(char* sz_Result,char* sz_OutPath); // it  does what its name says
void gotoxy(int x, int y);            // sets the cursor position
int ReadDiskB2B();
int Format0(char *sz_F, int iX);        // leads with zeroes the sz_X
int TEST__ReadHeader();                 // Welcome in the TEST__Area!!!!!

//BUGS/////////////////////////////////////////////////////////////////////


//REM:OVALS ///////////////////////////////////////////////////////////////
//REM:D01 // DEBUGS


//////////////////////  PROGRAM    ////////////////////////////////////////

int main(int argh, char *argv[])
{
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////            
    // REM:D02 TEST__ReadHeader();
    // REM:D01 ReadDiskB2B();
    // REM:D00 return 18;
    Proceed(); 
    
    
    printf("\n");
    system("pause");    
    return 0;
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////            
    
}

int ReadDiskB2B()          // reads the HDD octet with octet
{
    // 
    unsigned long int      i;  // reads the position in the file (really necessary??)
    char     *drv="\\\\.\\A:"; 
    char     *dsk="\\\\.\\PhysicalDrive0";
    char     *fnm="D:\\TMP\\MatrikonOPCHistoryLinkUserManual.pdf";
    char     *fou="D:\\TMP\\TEST.PDF";
    char      c;
    
    ifstream f01(fnm, ios::in |ios::binary);
    ofstream f02(fou, ios::out|ios::binary);
    if(f01){
        while(f01)
        {
            f01.get(c);
            f02<<c;        
        }
        printf("\n");
        return i;
        f01.close();
    }
    else
    {
        cout<<"could not open file: \""<<fnm<<"\""<<endl;
    }
    
    return 0;
    
}

int Proceed()
{
    const char     *drv="\\\\.\\F:"; 
    const char     *dsk="\\\\.\\PhysicalDrive0";
    char     *buff=new char[SEC_SIZE];
    LONG      nsect=0;
    PLONG     offset=0;
    char     *sz_Result;         // the content of the read sector - to seek for JPG headers
    char      sz_DrvScan[0xff];        //the drive to scan (like "\\.\A:")
    char      sz_OutPath[0xff];        //the output file name (current)

    memset(sz_DrvScan,'\0',sizeof(sz_DrvScan));        
    memset(sz_OutPath,'\0',sizeof(sz_OutPath));    
    printf("Enter the drive to scan (like \"\\\\.\\PhysicalDrive0\" or \"\\\\.\\A:\"): ");
    scanf("%s", sz_DrvScan);
    if(strlen(sz_DrvScan)<2)        memcpy(sz_DrvScan,"\\\\.\\G:",6);
    cout<<"\""<<sz_DrvScan<<"\" used"<<endl;
    
    printf("Enter the output path: ");
    scanf("%s", sz_OutPath);
    if(strlen(sz_OutPath)<2)        memcpy(sz_OutPath,"D:\\TMP\\OUT~",11);
    cout<<"\""<<sz_OutPath<<"\" used"<<endl;
    
    for(int q=0;q<=4;q++)
    {
        if(ReadSect(sz_DrvScan,buff,nsect++,offset)==0x12)
        {
            printf("Another application is probably already reading from disk\n");
            return 0xda;
        }
        /////////TODO: Something with the buffer///////////////////////////////////////////
        cout<<"\"SaveJPGFile()\" aufgerufen!"<<endl;
        SaveJPGFile(buff,sz_OutPath);   // it does what its name says
        //        cout<<char(13);
        printf("Reading offset nr. %04d\t Sector nr. %04d\n", offset, nsect);
        ///////////////////////////////////////////////////////////////////////////////////
    }
    if((unsigned char)buff[510]==0x55 && (unsigned char)buff[511]==0xaa)
    {
        printf("Disk is bootable\n");
    }

    return 0; 

}


short ReadSect(
               const char     *_dsk,    // disk to access
               char           *_buff,         // buffer where sector will be stored
               LONG            _nsect,   // sector number, starting with 0
               PLONG           _offset // sector offset
               )
{
    DWORD           dwRead;   
    HANDLE          hDisk;
    
    hDisk=CreateFile(_dsk,GENERIC_READ,FILE_SHARE_VALID_FLAGS,0,OPEN_EXISTING,0,0);
    if(hDisk==INVALID_HANDLE_VALUE) // this may happen if another program is already reading from disk
    {  
        cout<<"invalid disk: \""<<_dsk<<endl;
        CloseHandle(hDisk);
        return 0x12;
    }
    SetFilePointer(hDisk,_nsect*SEC_SIZE,_offset,FILE_BEGIN); // which sector to read
    ReadFile(hDisk,_buff,SEC_SIZE,&dwRead,0);  // read sector
    for(int i=0;i<=0xffff;i++){
        if(_buff[i]>10) cout<<_buff[i];
    }
    cout<<endl;
    CloseHandle(hDisk);
    return 0;
    
}


int SaveJPGFile(char* sz_Result,char* sz_OutPath)
{
    static 
    unsigned int      intFNo;                    // file number
    unsigned long     i;                         // the position in the current byte in the buffer "sz_Result";
    static
    unsigned long     j;                         // the position in the output string of file;
    char             *sz_FNo;                    // file number again in string format;
    char             *sz_ResJPG;                 // the part of sz_Result containing JPG information
    char              sz_Hea[]={0xff,0xd8,0xff,0xe0,0x00,0x10};           // JPG Header;
    char              sz_Heb[]={0xff,0xd8,0xff,0xe1,0x00,0x10};           // JPG Header another version
    char              sz_Hec[]={0xff,0xd8,0xff,0xe2,0x00,0x10};           // JPG Header another version    
    char              sz_Foo[]={0xff,0xd9};                               // JPG Footer;
    char              sz_Fop[]={0xff,0xda};                               // JPG Footer another version;
    char              sz_Foq[]={0xff,0xdb};                               // JPG Footer another version;
    static bool       booOUT;                    // register JPEG;
    static bool       booNEW;
    
                        // register NEW JPEG
    string            strFNo;                    // the part of the output file name in number format;
    string            strFZo;                    // the part of the output file name;
    string            strOutFile;                // output file name;
    ofstream          filRec;                       // output file stream;
    
    booOUT=false;  
    sz_ResJPG=(char*)malloc(MAX_JPEG);
    memset(sz_ResJPG,'\0', MAX_JPEG);          // Allocate memory for JPEG file
    for(i=0;i<=SEC_SIZE;i++)
    {   // do something with the sz_result, one by one
        if( // replace with memcmp() // after elliminating all bugs
           (sz_Hea[0]==sz_Result[i+0])&&
           (sz_Hea[1]==sz_Result[i+1])&&
           // (sz_Hea[2]==sz_Result[i+2])&&
           // (sz_Hea[3]==sz_Result[i+3])&&
           // (sz_Hea[4]==sz_Result[i+4])&&                      
           // (sz_Hea[5]==sz_Result[i+5])&&           
           true 
          ) 
        {
            printf("BOF@%08d\n", i);
            booOUT=true;
        }
        if(
           (sz_Foo[0]==sz_Result[i-2])&&
           (sz_Foo[1]==sz_Result[i-1])&&
           true
          ) 
        {
            printf("EOF@%08d\n", i);
            booOUT=false;
        }
        return 0x0012;    
        if(booOUT==true)  // if header detected and no eojpg detected
        {
            if(j==0)  // first entry; open the file; sets the output file name, opens it and so on
            {
                // DEBUG INFO HERE.....
                intFNo++;                            // starting from one
                sz_FNo=(char*)malloc(0x09);
                memset(sz_FNo,'\0',0xff);                       // allocate memory for file name (255 chars)
                itoa(intFNo,sz_FNo,10);                         // construct file name
                strFNo=(string)sz_FNo;                          // construct file name
                int nZ=strlen(sz_FNo);                          // formatting u.s.w.
                nZ=6-nZ;                                        
                strFZo="";
                for(int q=1;q<=nZ;q++) strFZo+="0";
                strFNo=strFZo+strFNo;                           
                strOutFile=(string)sz_OutPath+strFNo+(string)".TXT";         // ok; output path- and filename composed
                
                memset(sz_FNo,0,0xff);                                       // file name ?? to understand what I did... 
                sz_ResJPG=(char*)malloc(MAX_JPEG);             // allocate memory for JPG file (4 MB MAX_JPEG)
                memset(sz_ResJPG,'\0',MAX_JPEG);      
            }
            memcpy(sz_Result+i,sz_ResJPG+j,1);        // copies the "i" Byte from buffer to the "j" byte of the
            if(sz_ResJPG[i]>12)            cout<<sz_ResJPG+i;                     // it was just for debugging
            j++;
        }       
        else    // close the file and reset the "j";
        {
            if(j>0)
            { // save the file
                
                filRec.open(strOutFile.c_str(),ios::out|ios::binary);
                cout<<"here is it!!!";
                filRec.write(sz_ResJPG,j);
                filRec.close();
            }
            j=0;
        
        }            
        if(booOUT==true)
        {   // this means: new header found, without to find a EOF before!
            printf("NOF@%08d\n", i);
            cout<<"NEW ";
            j=0;                      // resetting j to force the program to open new file to be saved
        }
        else
        {
            printf("BOF@%08d\n", i);
            booOUT=true;
        }
        

    }
    return  0;
}

int TEST__ReadHeader()
{
    ifstream     f;
    int          i;
    char         c;
    
    f.open("D:\\tmq\\Afere\\102839\\5409502.jpg",ios::in|ios::binary);
    for(i=0;i<=3;i++)
    {
        f.read(&c,1);
        printf("%d ",c);
    }
    return i;
    
}


int Format0(char *sz_F, int iX)
{
    //
    char          *sz_IX;   //the string version of iX,
    int            i,j;     //iterators / / 
    
    itoa(iX, sz_IX, 10);
    while(sz_IX[i]!='\0') i++;
    j=sizeof(sz_F);
    j-=i;
    i=0;
    sz_F[j]='\0';
    while(sz_IX[i]!='\0')
    {
        memcpy(sz_F+j+i,sz_IX+i,1);
        i++;
    }
    return i;

}


void gotoxy(int x, int y) // sets the cursor position
{
    // sets the cursor to a specified position on the screen (what a joy - haa-ha!)
    COORD coord;
    coord.X = x;
    coord.Y = y;

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}


/***********************************************************************************************\
*                                                                                               *
*               ~~END OF FILE                                                                   *
*                                                                                               *
\***********************************************************************************************/

