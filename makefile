encode: encode.c stegano.c
	gcc -o encode.exe encode.c stegano.c

decode: decode.c stegano.c
	gcc -o decode.exe decode.c stegano.c

clean:
	rm *.exe *.stackdump *.log