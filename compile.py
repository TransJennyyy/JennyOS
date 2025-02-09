from os import listdir, chdir, system, remove, mkdir
from os.path import exists, join, isfile

from sys import platform, exit, argv
if exists("os.bin"):
    system("rm os.bin")

BootLoaderAsmFile = join("boot","boot.asm")
KernelFilesDir = "Kernel"
KernelFilesToLink = []


if not exists("BuildFiles"):
    mkdir("BuildFiles")
else:
    if platform == "win32":
        system("wsl rm -rf BuildFiles");
    else:
        system("rm -rf BuildFiles")
    mkdir("BuildFiles")

def GetOutDirrectory(path: str, FileEnding: str) -> str:
    path = path.split('.')
    path.pop()
    path = ''.join(path)
    path = path.replace('/', '.').replace('\\','.')
    path += f".{FileEnding}"
    return join("BuildFiles", path)

def GetFileEnding(path:str):
    path = path.split('.')
    return path.pop()

def CompileNasmFile(Path, Format):
    system(f"nasm {Path} -f {Format} -o {GetOutDirrectory(Path, Format)}")
    if not exists(GetOutDirrectory(Path, Format)):
        print("Error compiling");
        exit(1);


def CompileAllFilesInDir(path):
    for file in listdir(path):
        if isfile(join(path, file)):
           FileEnd = GetFileEnding(join(path,file))
           if FileEnd == "cpp":
                CompileCFile(join(path, file))
           if FileEnd == "asm":
               CompileNasmFile(join(path, file), 'elf32')
               KernelFilesToLink.append(GetOutDirrectory(join(path,file), 'elf32'))
        else:
            CompileAllFilesInDir(join(path, file))


def LinkKernelFile():
    global KernelFilesToLink
    ReorderedList = []
    for file in KernelFilesToLink:
        if(file == join("BuildFiles","Kernel.kernel.elf32")):
            ReorderedList.append(file)
    
    for file in KernelFilesToLink:
        if(file != join("BuildFiles","Kernel.kernel.elf32")):
            ReorderedList.append(file)
    KernelFilesToLink = ReorderedList
    print(KernelFilesToLink)
    TmpPath = join('BuildFiles', 'Kernel.elf').replace("\\", "/")
    if(platform != "win32"):
        TmpPath = join('BuildFiles', 'Kernel.o');
    LdCommmand = f"ld -o {TmpPath}  -m elf_i386 -Ttext 0x180000 "
    if(platform == "win32"):
        LdCommmand += "--oformat binary "
    for file in KernelFilesToLink:
        LdCommmand += f" {file} "
    if platform == "win32":
        LdCommmand = LdCommmand.replace("\\", "/")
        print(LdCommmand)
        system(f"wsl {LdCommmand}")
    else:
        print(LdCommmand);
        system(LdCommmand)
        system(f"objcopy -O binary {join('BuildFiles', 'Kernel.o')} {join('BuildFiles', 'Kernel.elf')}")
        system(f"objcopy --only-keep-debug {join('BuildFiles', 'Kernel.o')} Kernel.debug")
    if not exists(join("BuildFiles", "Kernel.elf")):
        print("error linking")
        exit(1)


def ExpandKernelToSize(NumberBytes):
    f = open(join("BuildFiles", "Kernel.elf"), 'rb')
    KernelNew = bytearray(f.read())
    f.close()
    if(len(KernelNew) > NumberBytes):
        raise Exception("Kernel Too Big");
    while len(KernelNew) != NumberBytes:
        KernelNew.append(0)
    return KernelNew

def CompileCFile(path):
    path = path.replace("\\", "/")
    if platform == "win32":
        #print(f"wsl g++ -o {GetOutDirrectory(path, 'elf')} -no-pie -fno-pic -ffreestanding -e KernelEntry -nostartfiles {path}")
        OutDir = GetOutDirrectory(path, 'elf').replace('\\', '/')
        system(f"wsl g++ {path} -fno-common -fno-strict-aliasing -m32 -ffreestanding -c -o {OutDir} -fno-pie -fno-builtin  ")
    else:
        OutDir = GetOutDirrectory(path, 'elf');
        system(f"g++ {path} -fno-stack-protector -fno-strict-aliasing -fno-common -m32 -ffreestanding -c -o {OutDir} -fno-pie -fno-builtin -nostartfiles -nolibc -nodefaultlibs -nostdlib  -mno-sse ") #-nostdlib++
    if not exists(GetOutDirrectory(path, 'elf')):
        print("Failed Compile")
        exit(1)
    KernelFilesToLink.append(GetOutDirrectory(path, 'elf'))
def SpiltKernelFiles():
    Kernel = open(join("buildfiles", "kernel.elf"), 'rb').read()
    SegmentSizes = [512] # all after is put into one chunk
    NumberBytesTakenBySegments = 0
    Segments = []
    KernelSegIndex = 0
    for seg in SegmentSizes:
        tmpseg = bytearray()
        for byteindex in range(seg):
            tmpseg.append(Kernel[NumberBytesTakenBySegments+byteindex])
        open(join("buildfiles", f"Kernel.{KernelSegIndex}"), 'wb').write(tmpseg)
        KernelSegIndex+=1
        NumberBytesTakenBySegments += seg
    
    FinnalSeg = bytearray()
    for i in range(len(Kernel)-NumberBytesTakenBySegments):
        FinnalSeg.append(Kernel[NumberBytesTakenBySegments+i])
    open(join("buildfiles", f"Kernel.{KernelSegIndex}"), 'wb').write(FinnalSeg)


        
CompileNasmFile(BootLoaderAsmFile, 'bin')
CompileNasmFile("KernelStarter.asm", 'bin')
CompileAllFilesInDir(KernelFilesDir)

LinkKernelFile()

Kernel =  ExpandKernelToSize(512*256)
print(f"Kernel Size:{len(Kernel)/512}")
print("loading bootloader to memory")
bootloader = open(GetOutDirrectory(BootLoaderAsmFile, 'bin'), 'rb').read()

FinnalOs = bytearray()
for byte in bootloader:
    FinnalOs.append(byte)    

print("combining Kernel and bootloader")

for byte in open(GetOutDirrectory("KernelStarter.asm", 'bin'), 'rb').read():
    FinnalOs.append(byte)

for byte in Kernel:
    FinnalOs.append(byte)


system("python3 FileSystem/CreateSystem.py")

print("Reading FS...", end="");
fs = open("FS.bin", 'rb').read()
for byte in fs:
    FinnalOs.append(byte)
print("Done")


#remove("os.bin")
f = open("os.bin", 'wb')
f.write(FinnalOs)


if len(argv) > 1:
    if argv[1] == 'NoQemu':
        f.close()
        exit(0)
print("Expanding FS...", end="", flush=True)
f.truncate((512*258) + (8*1024*1024*1024))
f.close()
print("Done")
if exists("os.bin"):
    system("qemu-system-x86_64 -m 1G -smp 4 -d guest_errors,int -D Qemu.log -drive file=os.bin,format=raw ")
