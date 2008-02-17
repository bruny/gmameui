#ifndef __UNZIP_H
#define __UNZIP_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* not defined anywhere else */
typedef signed char	INT8;
typedef signed short	INT16;
typedef signed int	INT32;
typedef unsigned char	UINT8;
typedef unsigned short	UINT16;
typedef unsigned int	UINT32;
/***************************************************************************
 * Support for retrieving files from zipfiles
 ***************************************************************************/

struct zipent {
	UINT32	cent_file_header_sig;
	UINT8	version_made_by;
	UINT8	host_os;
	UINT8	version_needed_to_extract;
	UINT8	os_needed_to_extract;
	UINT16	general_purpose_bit_flag;
	UINT16	compression_method;
	UINT16	last_mod_file_time;
	UINT16	last_mod_file_date;
	UINT32	crc32;
	UINT32	compressed_size;
	UINT32	uncompressed_size;
	UINT16	filename_length;
	UINT16	extra_field_length;
	UINT16	file_comment_length;
	UINT16	disk_number_start;
	UINT16	internal_file_attrib;
	UINT32	external_file_attrib;
	UINT32	offset_lcl_hdr_frm_frst_disk;
	char*   name; /* 0 terminated */
};

typedef struct {
	char* zip; /* zip name */
	FILE* fp; /* zip handler */
	long length; /* length of zip file */

	char* ecd; /* end_of_cent_dir data */
	unsigned ecd_length; /* end_of_cent_dir length */

	char* cd; /* cent_dir data */

	unsigned cd_pos; /* position in cent_dir */

	struct zipent ent; /* buffer for readzip */

	/* end_of_cent_dir */
	UINT32	end_of_cent_dir_sig;
	UINT16	number_of_this_disk;
	UINT16	number_of_disk_start_cent_dir;
	UINT16	total_entries_cent_dir_this_disk;
	UINT16	total_entries_cent_dir;
	UINT32	size_of_cent_dir;
	UINT32	offset_to_start_of_cent_dir;
	UINT16	zipfile_comment_length;
	char*	zipfile_comment; /* pointer in ecd */
} ZIP;

/* Opens a zip stream for reading
   return:
     !=0 success, zip stream
     ==0 error
*/
ZIP* openzip(const char* path);

/* Closes a zip stream */
void closezip(ZIP* zip);

/* Reads the current entry from a zip stream
   in:
     zip opened zip
   return:
     !=0 success
     ==0 error
*/
struct zipent* readzip(ZIP* zip);

/* Resets a zip stream to the first entry
   in:
     zip opened zip
   note:
     ZIP file must be opened and not suspended
*/
void rewindzip(ZIP* zip);

/* Reads a zipentry
* returns the entry or NULL if anything went wrong.
* Returns: a char buffer allocated with g_malloc.
*/
char *read_zipentry(ZIP* zip, struct zipent* ent);


#ifdef __cplusplus
}
#endif

#endif
