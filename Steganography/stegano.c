/*
 * hidefy.c - Simple Steganography Utility
 *
 * MIT License
 *
 * Copyright (c) 2019 Nikola Knezevic
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#define MAGIC_VALUE	0x4d42
#define BIT_DEPTH	24	

#pragma pack (push, 1)

struct BMPHeader {
	uint16_t type;
	uint32_t size;
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t offset;
	uint32_t header_size;
	int32_t  px_width;
	int32_t  px_height;
	uint16_t num_planes;
	uint16_t bit_depth;
	uint32_t compression;
	uint32_t img_size;
};

#pragma pack (pop)

struct BMPHeader header;

/* extract and print to stdout */
void extract (FILE * fp)
{
	size_t i, j, n = 100;
	uint8_t * buffer, byte, low;
	buffer = (uint8_t *) malloc (sizeof (*buffer) * n);

	for (i = 0 ;; i++)
	{
		for (j = low = 0; j < 4; j++)
		{
			fread (&byte, sizeof (byte), 1, fp);
			low |= (byte & 0x03) << j * 2;
		}
		buffer[i] = low;
		if (low == '\0') break;
		/* expand buffer if neccessary */
		if (i == n - 1)
		{
			n = n * 2;
			buffer = (uint8_t *) realloc (buffer, n);
		}
	}

	printf ("%s\n", buffer);
	free (buffer);
}

/* embed message using LSB technique */
void embed (char * msg, FILE * in, FILE * out)
{
	uint8_t i, low, byte;
	uint8_t bitmask[] = { 0x03, 0x0C, 0x30, 0xC0 };

	for (;;)
	{
		for (i = 0; i < 4; i++)
		{
			low = (*msg & bitmask[i]) >> i * 2;
			fread (&byte, sizeof (byte), 1, in);
			low += byte & 0xFC;
			fwrite (&low, sizeof (low), 1, out);
		}		
		if (*msg++ == '\0') break;
	}
}

/* duplicate cover image */
int copy (FILE * src, FILE * dest)
{
	char buffer[4096];
	size_t bytes_read;

	/* read from beginning */
	fseek (src, 0, SEEK_SET);
	while (!feof (src))
	{
		bytes_read = fread (buffer, sizeof (char), 4096, src);
		if (bytes_read < sizeof (buffer))
		{
			if (ferror (src))
			{
				fprintf (stderr, "Error reading from source\n");
				fclose (src); fclose (dest);
				return 0;
			}
		}
		if (fwrite (buffer, sizeof (char), bytes_read, dest) != bytes_read)
		{
			fprintf (stderr, "Error writing to destination\n");
			fclose (src); fclose (dest);
			return 0;
		}
	}
	return 1;
}

/* basic input file test */
int is_valid (FILE * fp)
{
	if (fp == NULL) 
	{
		fprintf (stderr, "Error opening cover image\n");
		return 0;
	}

	/* populate global BMPHeader struct */
	fread (&header, sizeof (header), 1, fp);

	if (header.type != MAGIC_VALUE) // BM
	{
		fprintf (stderr, "Not a bitmap\n");
		fclose (fp);
		return 0;
	}

	if (header.bit_depth != BIT_DEPTH) // truecolor
	{
		fprintf (stderr, "Cover image must have 24-bit color depth\n");
		fclose (fp);
		return 0;
	}
	return 1;
}

/* append _stego suffix to image filename */
char * get_filename (char * cover)
{
	char * str = (char *) malloc (strlen (cover) + 7);
	char * ptr = str;
	while (*cover != '.')
		*ptr++ = *cover++;
	strcpy (ptr, "_stego.bmp");
	return str;			
}

/* calculate if embedding is possible */
int has_space (char * msg, FILE * fp)
{
	size_t bit_pairs = 4 * (strlen (msg) + 1);
	if (bit_pairs > header.img_size)
	{
		fprintf (stderr, "Not enough space\n");
		fclose (fp);
		return 0;
	}
	return 1;	
}

int hide (char * msg, char * cover, char * out)
{
	char * out_name;
	FILE * cover_fp, * out_fp;

	cover_fp = fopen (cover, "r");
	if (!is_valid (cover_fp) || !has_space (msg, cover_fp))
		return EXIT_FAILURE;	
	
	/* append suffix if -o flag not specified */
	out_name = out ? out : get_filename (cover);
	out_fp = fopen (out_name, "wr");

	if (!copy (cover_fp, out_fp))
		return EXIT_FAILURE;

	/* jump to pixel array and start embedding */
	fseek (cover_fp, header.offset, SEEK_SET);
	fseek (out_fp,   header.offset, SEEK_SET);
	embed (msg, cover_fp, out_fp);
	
	printf ("File '%s' successfully created\n", out_name);

	/* cleanup */
	if (!out) free (out_name);
	fclose (cover_fp); fclose (out_fp);
	return EXIT_SUCCESS;
}

int show (char * cover)
{
	FILE * fp = fopen (cover, "r");
	if (!is_valid (fp))
		return EXIT_FAILURE;

	/* jump to pixel array */
	fseek (fp, header.offset, SEEK_SET);
	extract (fp);
	fclose (fp);
	return EXIT_SUCCESS;
}

void print_usage (char * name)
{
	printf ("Usage:\n");
	printf ("  %s -m string <cover_img> [-o <outfile>]\n", name);
	printf ("  %s -x <stego_img>\n", name);
	printf ("\nOptions:\n");
	printf ("  -m	Secret text message to be embedded\n");
	printf ("  -o	Optional path to the output bitmap file\n");
	printf ("  -x	Extracts text from stego image to stdout\n"); 
}

int main (int argc, char * argv[])
{
	int c, xflag = 0;
	char * cover_img, * secret_msg;
	char * out_file = NULL;

	while ((c = getopt (argc, argv, "m:o:x")) != -1)
	{
		switch (c)
		{
			case 'm':
				secret_msg = optarg;
				break;
			case 'o':
				out_file = optarg;
				break;
			case 'x':
				xflag = 1;
				break;
			case '?':
				return EXIT_FAILURE;
			default:
				abort ();	
		}	
	}		

	/* handle empty args, too many args, etc. */
	if (optind == 1 || optind + 1 != argc 
			|| (xflag && optind != 2))
	{
		print_usage (basename (argv[0]));
		return EXIT_FAILURE;
	}

	/* set remaining non-optional arguments */
	cover_img = argv[optind];

	if (xflag)
		return show (cover_img);
	else
		return hide (secret_msg, cover_img , out_file);
}

