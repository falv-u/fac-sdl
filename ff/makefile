TARGET = generador_mapas

CC     = c99
# Se elimina -I./tools porque los .h están en el mismo directorio
CFLAGS = -Dkiss_fft_scalar=float -Wall -Wextra
LDLIBS = -lm

# Los objetos se compilan a partir de los .c en la raíz
OBJS   = analizador.o kiss_fft.o kiss_fftr.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

# Regla de inferencia POSIX estricta para todos los archivos .c
.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET) $(OBJS)
