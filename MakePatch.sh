rm os.bin
rm FS.bin
rm Qemu.log
rm patch.patch
rm -rf BuildFiles
git add .
git commit -m "Auto Add Script"

git format-patch -1 HEAD --stdout > patch.patch
