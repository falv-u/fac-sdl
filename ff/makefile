.POSIX:

CC = cc
CFLAGS = -O2 -Wall -Wextra -std=c99 -Dkiss_fft_scalar=float
LDFLAGS = -lm

TARGET = generador_mapas
OBJS = analizador.o kiss_fft.o kiss_fftr.o stb_vorbis.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

analizador.o: analizador.c kiss_fft.h tools/kiss_fftr.h
	$(CC) $(CFLAGS) -I. -Itools -c analizador.c

kiss_fft.o: kiss_fft.c kiss_fft.h _kiss_fft_guts.h
	$(CC) $(CFLAGS) -I. -c kiss_fft.c

kiss_fftr.o: tools/kiss_fftr.c tools/kiss_fftr.h kiss_fft.h _kiss_fft_guts.h
	$(CC) $(CFLAGS) -I. -Itools -c tools/kiss_fftr.c

stb_vorbis.o: stb_vorbis.c
	$(CC) $(CFLAGS) -c stb_vorbis.c -DSTB_VORBIS_HEADER_ONLY=0

clean:
	rm -f $(OBJS) $(TARGET)
