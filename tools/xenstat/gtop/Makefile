CC		= gcc
#CC		= /opt/arm-linux-4.1.1/bin/arm-linux-gcc
OBJS	= main.o common.o gtop.o mygen.o 
SRCS	= $(OBJS:.o=.c)
TARGET	= libgtop.so
LDFLAGS = -L../../xenstore
CFLAGS	= -Wall -g -L./ \
		  -shared \
		  -DG_DISABLE_DEPRECATED \
		  -DGDK_DISABLE_DEPRECATED \
		  -DGDK_PIXBUF_DISABLE_DEPRECATED \
		  -DGTK_DISABLE_DEPRECATED
export PKG_CONFIG_PATH=/usr/xocean/lib/pkgconfig

all: $(OBJS)
	$(CC) $(OBJS) libxenstat.a $(LDFLAGS) -lxenstore -o $(TARGET) $(CFLAGS) `pkg-config gtk+-2.0 --cflags --libs gthread-2.0`
	cp $(TARGET) /tftpboot/
	
clean:
	rm -f $(OBJS) $(TARGET)

mygen.o:
	$(CC) -c mygen.c -o mygen.o $(CFLAGS) `pkg-config gtk+-2.0 --cflags`

.c.o: 
	$(CC) -c $< $(CFLAGS) `pkg-config gtk+-2.0 --cflags`

