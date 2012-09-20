/*
 * common/hexdump.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */

#include <xen/kernel.h>

/**
 * hex_dump_to_buffer - convert a blob of data to "hex ASCII" in memory
 * @buf: data blob to dump
 * @len: number of bytes in the @buf
 * @rowsize: number of bytes to print per line; must be 16 or 32
 * @groupsize: number of bytes to print at a time (1, 2, 4, 8; default = 1)
 * @linebuf: where to put the converted data
 * @linebuflen: total size of @linebuf, including space for terminating NUL
 * @ascii: include ASCII after the hex output
 *
 * hex_dump_to_buffer() works on one "line" of output at a time, i.e.,
 * 16 or 32 bytes of input data converted to hex + ASCII output.
 *
 * Given a buffer of u8 data, hex_dump_to_buffer() converts the input data
 * to a hex + ASCII dump at the supplied memory location.
 * The converted output is always NUL-terminated.
 *
 * E.g.:
 *   hex_dump_to_buffer(frame->data, frame->len, 16, 1,
 *       linebuf, sizeof(linebuf), true);
 *
 * example output buffer:
 * 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f  @ABCDEFGHIJKLMNO
 */
void hex_dump_to_buffer(const void *buf, size_t len, int rowsize,
                        int groupsize, char *linebuf, size_t linebuflen,
                        bool ascii)
{
   const u8 *ptr = buf;
   u8 ch;
   int j, lx = 0;
   int ascii_column;

   if (rowsize != 16 && rowsize != 32)
      rowsize = 16;

   if (!len)
      goto nil;
   if (len > rowsize)      /* limit to one line at a time */
      len = rowsize;
   if ((len % groupsize) != 0)   /* no mixed size output */
      groupsize = 1;

   switch (groupsize) {
   case 8: {
      const u64 *ptr8 = buf;
      int ngroups = len / groupsize;

      for (j = 0; j < ngroups; j++)
         lx += scnprintf(linebuf + lx, linebuflen - lx,
                         "%s%16.16llx", j ? " " : "",
                         (unsigned long long)*(ptr8 + j));
      ascii_column = 17 * ngroups + 2;
      break;
   }

   case 4: {
      const u32 *ptr4 = buf;
      int ngroups = len / groupsize;

      for (j = 0; j < ngroups; j++)
         lx += scnprintf(linebuf + lx, linebuflen - lx,
                         "%s%8.8x", j ? " " : "", *(ptr4 + j));
      ascii_column = 9 * ngroups + 2;
      break;
   }

   case 2: {
      const u16 *ptr2 = buf;
      int ngroups = len / groupsize;

      for (j = 0; j < ngroups; j++)
         lx += scnprintf(linebuf + lx, linebuflen - lx,
                         "%s%4.4x", j ? " " : "", *(ptr2 + j));
      ascii_column = 5 * ngroups + 2;
      break;
   }

   default:
      for (j = 0; (j < len) && (lx + 3) <= linebuflen; j++) {
         ch = ptr[j];
         lx += scnprintf(linebuf + lx, linebuflen - lx,
                         "%2.2x ", *(ptr + j));
      }
      if (j)
         lx--;

      ascii_column = 3 * rowsize + 2;
      break;
   }
   if (!ascii)
      goto nil;

   while (lx < (linebuflen - 1) && lx < (ascii_column - 1))
      linebuf[lx++] = ' ';
   for (j = 0; (j < len) && (lx + 2) < linebuflen; j++) {
      ch = ptr[j];
      linebuf[lx++] = (isascii(ch) && isprint(ch)) ? ch : '.';
   }
nil:
   linebuf[lx++] = '\0';
}

/*
 * Local variables:
 * eval: (xen-c-mode)
 * End:
 */
