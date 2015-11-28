malfs : malfs.c malfs.h
		gcc -D_FILE_OFFSET_BITS=64 -o malfs malfs.c -lfuse


			
