#ifndef HX_FILE_H
#define HX_FILE_H

#include <stdbool.h>

#define HX_FILEITER_CHUNK_SIZE 1024
#define HX_FILEITER_RESIZE_FACTOR 1.5


typedef struct
{
	FILE *file;
	long file_size;
	char *line_buffer;
	long bytes_read;
	bool eof;
	bool initialized;
} hx_file_iter_t;


off_t
HxFile_get_size(const hx_str_t *filename);


hx_seq_t *
HxFile_get_filenames_in_folder(hx_str_t *foldername);


hx_file_iter_t *
HxFileIter_new(FILE *file);


void
HxFileIter_initialize(hx_file_iter_t *self);


bool
HxFileIter_next(hx_file_iter_t *self);


bool
HxFileIter_read_next_line(hx_file_iter_t *self);


void
HxFileIter_rewind(hx_file_iter_t *self);


hx_str_t *
HxFileIter_get(hx_file_iter_t *self);


void
HxFileIter_del(hx_file_iter_t *self);

#endif /* HX_FILE_H */
