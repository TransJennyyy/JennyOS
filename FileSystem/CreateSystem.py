from os import system, listdir, path
from sys import platform
from numpy import uint32

class FileSystem:
    FILESYSTEMDIR = path.join("FileSystem","drive")
    FSSETSIZE = 8
    FileSystemFiles = []
    FileSystemDirs = []

    def ScanFiles(self, StartPath, AddPath):
        Files = []
        Dirs = []
        

        for file in listdir(StartPath):
            PathUsing = ""
            if(AddPath):
                PathUsing = path.join(StartPath, file)
            else:
                PathUsing = file
            if path.isfile(path.join(StartPath, file)):
                Files.append(PathUsing)
            else:
                Dirs.append(PathUsing)
                for files in self.ScanFiles(path.join(StartPath, file), False):
                       Files.append( path.join(file, files))

        self.FileSystemFiles = Files
        self.FileSystemDirs = Dirs
        return Files
    
    FileBins = []
    def LoadAllFiles(self):
        FileBins = [];
        for file in self.FileSystemFiles:
            FileBins.append(open(path.join(self.FILESYSTEMDIR ,file), 'rb').read())
            self.FileCheckSums.append(self.CheckSum(FileBins[len(FileBins)-1]))
        self.FileBins = FileBins

    def RoundUp(self, Number, RoundPoint):
        while((Number/RoundPoint != int(Number/RoundPoint))):
            Number+=1;
        return Number

    FileIndexs = []
    def CreateFileHeaders(self):
        FileIndexNumber = 0;
        for i in range(len(self.FileBins)):
            
            Index = bytearray()

            Path = str(self.FileSystemFiles[i])
            Path = Path.replace("\\", "/")
            
            for byte in bytes(Path, 'UTF-8'):
                Index.append(byte)
            Index.append(0)

            Index.append((self.FileLBAs[i] & 0xff))
            Index.append((self.FileLBAs[i] >> 8) & 0xff)
            Index.append((self.FileLBAs[i] >> 16) & 0xff)
            Index.append((self.FileLBAs[i] >> 24) & 0xff)
            Index.append((self.FileLBAs[i] >> 32) & 0xff)
            Index.append((self.FileLBAs[i] >> 40) & 0xff)


            NumberSectors = int(self.RoundUp(len(self.FileBins[i]), 512)/512)

            Index.append((NumberSectors & 0xff))
            Index.append((NumberSectors >> 8) & 0xff)
            Index.append((NumberSectors >> 16) & 0xff)
            Index.append((NumberSectors >> 24) & 0xff)


            Index.append(0)
            
            CheckSum = self.FileCheckSums[i]

            Index.append((CheckSum & 0xff))
            Index.append((CheckSum >> 8) & 0xff)
            Index.append((CheckSum >> 16) & 0xff)
            Index.append((CheckSum >> 24) & 0xff)

            Index.append(0)
            Index.append(0)
            Index.append(0)

            FileIndexNumber +=1
            if(FileIndexNumber > ((len(self.FileSystemDirs) + len(self.FileSystemFiles))-1)):
                FileIndexNumber = 0
            Index.append((FileIndexNumber & 0xff))
            Index.append((FileIndexNumber >> 8) & 0xff)
            Index.append((FileIndexNumber >> 16) & 0xff)
            Index.append((FileIndexNumber >> 24) & 0xff)
            Index.append((FileIndexNumber >> 32) & 0xff)
            Index.append((FileIndexNumber >> 40) & 0xff)
            
            if(len(Index) > 512):
                raise OverflowError()
            while( len(Index) != 512 ):
                Index.append(0)
            self.FileIndexs.append(Index)

        for i in range(len(self.FileSystemDirs)):
            Index = bytearray()

            Path = str(self.FileSystemDirs[i])
            Path = Path.replace("\\", "/")
            
            for byte in bytes(Path, 'UTF-8'):
                Index.append(byte)
            Index.append(0)

            Index.append(0xff)
            Index.append(0xff)
            Index.append(0xff)
            Index.append(0xff)
            Index.append(0xff) # used to tell if a file or not
            Index.append(0xff)
            Index.append(0xff)
            Index.append(0xff)
            Index.append(0xff)
            Index.append(0xff)

            for j in range(8):
                Index.append(0)


            FileIndexNumber +=1
            if(FileIndexNumber > ((len(self.FileSystemDirs) + len(self.FileSystemFiles))-1)):
                FileIndexNumber = 0
            Index.append((FileIndexNumber & 0xff))
            Index.append((FileIndexNumber >> 8) & 0xff)
            Index.append((FileIndexNumber >> 16) & 0xff)
            Index.append((FileIndexNumber >> 24) & 0xff)
            Index.append((FileIndexNumber >> 32) & 0xff)
            Index.append((FileIndexNumber >> 40) & 0xff)

            if(len(Index) > 512):
                raise OverflowError()
            while( len(Index) != 512 ):
                Index.append(0)
            self.FileIndexs.append(Index)
            
    NextFileLBA = 0 
    def CreateFileLBAs(self):
        CurrentLBAOffset = (len(self.FileSystemFiles) + len(self.FileSystemDirs)) + 1 # offset per each file and the file system headers
        for i in range(len(self.FileBins)):
            self.FileLBAs.append(CurrentLBAOffset)
            print(f"Created LBA table at LBA:{CurrentLBAOffset} Of Size {self.RoundUp(len(self.FileBins[i]), 512)/512}")
            CurrentLBAOffset += (int(self.RoundUp(len(self.FileBins[i]), 512)/512))
        self.NextFileLBA = CurrentLBAOffset

            
    FileLBAs = []
            
    FileCheckSums = []
    def CheckSum(self, Bin):
        print(f"Adding {len(Bin)} Number Bytes")
        Sum  = uint32(0)
        for byte in Bin:
            Sum = uint32(Sum+byte)
        #    print(Sum)
        print(f"Got Checksum:{Sum}")
        return Sum
        

    #MetaData
    #Flags (1 byte)   |0:IsFree,1-7:unused
    #checksum (4 bytes) | check sum is calcuated by a xorshift

    #FS table type

    #String(NULL TER)                        | Path
    #48 bit LBA (only 32 bit on backup mode) | LBA
    #NumberSectors (32 bit)                  | Num Sectors
    #MetaData (8 bytes)                      | Meta Data

    def __init__(self):
        print(f"Creating FS Using {self.FILESYSTEMDIR} As Drive")

        if not path.exists(self.FILESYSTEMDIR):
            raise FileNotFoundError();

        self.ScanFiles(self.FILESYSTEMDIR, False);

        print(f"Files Creating:{self.FileSystemFiles}")
        print(f"Paths Creating:{self.FileSystemDirs}")

        print("Loading Files...", end="");
        self.LoadAllFiles()
        print("Done")

        self.CreateFileLBAs()

        self.CreateFileHeaders()


        FinnalFS =  bytearray()

        FSHEADER = "JOSFS"
        for byte in bytes(FSHEADER, 'UTF-8'):
            FinnalFS.append(byte)
        
        FSSTARTPOINT = 258

        
        FinnalFS.append(FSSTARTPOINT & 0xff)
        FinnalFS.append((FSSTARTPOINT >> 8) & 0xff)
        FinnalFS.append((FSSTARTPOINT >> 16)& 0xff)
        FinnalFS.append((FSSTARTPOINT >> 24) & 0xff)

        NextFreeLBA = self.NextFileLBA

        FinnalFS.append(NextFreeLBA & 0xff)
        FinnalFS.append((NextFreeLBA >> 8) & 0xff)
        FinnalFS.append((NextFreeLBA >> 16)& 0xff)
        FinnalFS.append((NextFreeLBA >> 24) & 0xff)
        FinnalFS.append((NextFreeLBA >> 32) & 0xff)
        FinnalFS.append((NextFreeLBA >> 40) & 0xff)

        while(len(FinnalFS) != 512):
            FinnalFS.append(0)

        for IndexTable in self.FileIndexs:
            for byte in IndexTable:
                FinnalFS.append(byte)

        for File in self.FileBins:
            print(f"Creating File At Offset {len(FinnalFS)/512}")
            for byte in File:
                FinnalFS.append(byte)
            for i in range(self.RoundUp(len(FinnalFS), 512)-len(FinnalFS)):
                FinnalFS.append(0)
    

        for i in range(3):
            self.FSSETSIZE*= 1024
    


        f = open("FS.bin", 'wb')
        f.write(FinnalFS)
        f.close()
        print("Made File system")

        
JexeDir = {
    "Apps/TestApp.asm":"/TestApp.jexe",
    "Apps/AnotherApp.asm":"/AnotherApp.jexe"
}

for file in JexeDir.keys():
    if not path.exists(file):
        raise FileNotFoundError()
    system(f"nasm {file} -f bin -o FileSystem/drive/{JexeDir[file]}")
    if(not path.exists(f"FileSystem/drive/{JexeDir[file]}")):
        raise FileNotFoundError()

FileSystem()
