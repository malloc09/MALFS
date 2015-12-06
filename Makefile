malfs : malfs.c malfs.h
		gcc -W -D_FILE_OFFSET_BITS=64 -o malfs malfs.c malfs.h -lfuse


			
