#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <glob.h>

#include "hx_exc.h"
#include "hx_str.h"
#include "hx_file.h"
#include "hx_path.h"
#include "hx_utils.h"


off_t
HxFile_get_size(const hx_str_t *filename)
{
	struct stat st;
	if (stat(filename->s, &st) == 0)
	{
		return st.st_size;
	}
	return -1;
}


/*
 * @return A hx_seq_t pointer with the filenames as hx_str_t pointer as elements, or NULL on error.
 */
hx_seq_t *
HxFile_get_filenames_in_folder(hx_str_t *foldername)
{
	hx_seq_t *seq = HxSeq_new(NULL);

	hx_str_t *wildcard = HxStr_new("*.*");
	hx_str_t *globstr = Hx_Path_join(foldername, wildcard);

	glob_t data;
	switch(glob(globstr->s, 0, NULL, &data))
	{
		case 0:
			break;
		case GLOB_NOSPACE:
			printf("%s", HxExc_get_error_name(HxMemoryAllocationError));
			exit(HxMemoryAllocationError);
		case GLOB_ABORTED:
			printf("%s", HxExc_get_error_name(HxIOError));
			exit(HxIOError);
		case GLOB_NOMATCH:
			printf("No files found\n");
			break;
		default:
			break;
	}

	int i;
	for(i = 0; i < data.gl_pathc; i++)
	{
		hx_str_t * fname = HxStr_new(data.gl_pathv[i]);
		HxSeq_append(seq, fname);
	}

	globfree(&data);
	HxStr_del(wildcard);
	HxStr_del(globstr);

	return seq;
}


hx_file_iter_t *
HxFileIter_new(FILE *file)
{
	hx_file_iter_t *self = (hx_file_iter_t *)Hx_Utils_allocate(sizeof(hx_file_iter_t));
	self->line_buffer = NULL;
	self->file = file;
	self->initialized = false;
	return self;
}


bool
HxFileIter_next(hx_file_iter_t *self)
{
	if (!self->initialized)
	{
		HxFileIter_initialize(self);
	}

	if (self->eof)
	{
		return false;
	}
	HxFileIter_read_next_line(self);
	return !self->eof;
}


/*
 * Read until next EOL.
 *
 * @param self A hx_file_iter_t instance.
 * @return Whether end of file was reached.
 *
 */
bool
HxFileIter_read_next_line(hx_file_iter_t *self)
{
	/* position read cursor in file at offset 'self->bytes_read' */
	if (fseek(self->file, self->bytes_read, SEEK_SET) == 0)
	{
		/* buffer for read chunks */
		char tmp_chunk_buffer[HX_FILEITER_CHUNK_SIZE];

		/* buffer for a whole line in the file */
		char *tmp_line_buffer = (char *)Hx_Utils_allocate(sizeof(char));
		tmp_line_buffer[0] = '\0';

		int i;
		int bytes_in_chunk;
		int buf_size = 0;
		long bytes_read_this_call = 0;
		bool line_end_found = false;
		while(!line_end_found)
		{
			/* read next chunk */
			bytes_in_chunk = fread(tmp_chunk_buffer, sizeof(char), HX_FILEITER_CHUNK_SIZE, self->file);
			i = 0;
			while(i < bytes_in_chunk)
			{
				/* look for line ending */
				line_end_found = (tmp_chunk_buffer[i] == '\n') || (tmp_chunk_buffer[i] == '\r');

				++i;

				if (line_end_found)
				{
					break;
				}
			}
			bytes_read_this_call += i;
			self->bytes_read += i;

			if (bytes_in_chunk < HX_FILEITER_CHUNK_SIZE && i >= bytes_in_chunk)
			{
				self->eof = true;
				line_end_found = true;
			}

			/* resize tmp_line_buffer if neccessary */
			if (bytes_read_this_call + HX_FILEITER_CHUNK_SIZE + 1 >= buf_size)
			{

				buf_size = (buf_size > HX_FILEITER_CHUNK_SIZE ? buf_size * HX_FILEITER_RESIZE_FACTOR : HX_FILEITER_CHUNK_SIZE) + 1;
				char *p = realloc(tmp_line_buffer, buf_size * sizeof(char));
				if (p == NULL)
				{
					printf("%s", HxExc_get_error_name(HxMemoryAllocationError));
					exit(HxMemoryAllocationError);
				}
				tmp_line_buffer = p;
			}

			/* copy tmp_chunk_buffer to tmp_line_buffer */
			strncat(tmp_line_buffer, tmp_chunk_buffer, i);
		}

		/* set self->line_buffer to tmp_line_buffer */
		self->line_buffer = tmp_line_buffer;
	}
	else
	{
		printf("%s: %s", HxExc_get_error_name(HxIOError), strerror(errno));
		exit(HxIOError);
	}
	return self->eof;
}


void
HxFileIter_initialize(hx_file_iter_t *self)
{
	self->bytes_read = 0;
	self->eof = false;
	if (fseek(self->file, 0, SEEK_END) != 0)
	{
		printf("%s", HxExc_get_error_name(HxIOError));
		exit(HxIOError);
	}
	self->file_size = ftell(self->file);
	if (fseek(self->file, 0, SEEK_SET) != 0)
	{
		printf("%s", HxExc_get_error_name(HxIOError));
		exit(HxIOError);
	}
	self->initialized = true;
}


void
HxFileIter_rewind(hx_file_iter_t *self)
{
	HxFileIter_initialize(self);
}


hx_str_t *
HxFileIter_get(hx_file_iter_t *self)
{
	return HxStr_new(self->line_buffer);
}


void
HxFileIter_del(hx_file_iter_t *self)
{
	free(self->line_buffer);
	free(self);
}
