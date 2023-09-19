path %path%;C:\cygwin\bin
set GCC_EXEC_PREFIX=c:/cygwin/usr
gcc -I..\ -c image_transfer.c -o image_transfer.obj -O7 -funroll-loops -march=i686
