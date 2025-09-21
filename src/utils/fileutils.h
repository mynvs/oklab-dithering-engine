#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#include <fcntl.h>
#include <io.h>

// cross-platform defines
#define mkdir(dir, mode) _mkdir(dir)
#define strdup _strdup

#else
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif

static char** list_files(const char* dir, unsigned int* len, const char* filterby) {
	char** files = NULL;
	*len = 0;
	char afp[512];

#ifdef _WIN32
	char fullpath[512];

	snprintf(fullpath, sizeof(fullpath), "%s/*", dir);
	WIN32_FIND_DATA fdt;
	HANDLE fhndl = FindFirstFile(fullpath, &fdt);

	if (fhndl == INVALID_HANDLE_VALUE) {
		return NULL;
	}
	while (FindNextFile(fhndl, &fdt) != 0) {
		if (strcmp(fdt.cFileName, ".") == 0 || strcmp(fdt.cFileName, "..") == 0)
			continue;
		snprintf(afp, sizeof(afp), "%s\\%s", dir, fdt.cFileName);
		if (fdt.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			unsigned int slen = 0;
			char** sf = list_files(afp, &slen, filterby);
			if (sf) {
				files = (char**)realloc(files, (*len + slen) * sizeof * files);
				for (unsigned int i = 0; i < slen; i++)
					files[*len + i] = sf[i];
				*len += slen;
				free(sf);
			}
		} else {
			if (!filterby || strstr(afp, filterby)) {
				files = (char**)realloc(files, (*len + 1) * sizeof(char*));
				files[*len] = strdup(afp);
				(*len)++;
			}
		}
	}
	FindClose(fhndl);
#else
	DIR* dirc;
	struct dirent* entry;

	if (!(dirc = opendir(dir)))
		return NULL;

	while ((entry = readdir(dirc)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		snprintf(afp, sizeof(afp), "%s/%s", dir, entry->d_name);
		if(entry->d_type == DT_DIR){
			unsigned int slen = 0;
			char** sf = list_files(afp, &slen, filterby);
			if(sf){
				snprintf(afp, sizeof(afp), "%s/%s", dir, entry->d_name);
				files = (char**)realloc(files, (*len + slen) * sizeof *files);
				for(unsigned int i = 0; i < slen; i++)
					files[*len + i] = sf[i];
				*len += slen;
				free(sf);
			}
		} else if (entry->d_type == DT_REG) {
			snprintf(afp, sizeof(afp), "%s/%s", dir, entry->d_name);
			if(!filterby || strstr(afp, filterby)){
    			files = (char**)realloc(files, (*len + 1) * sizeof *files);
    			files[*len] = strdup(afp);
				(*len)++;
			}
		}
	}
	closedir(dirc);
#endif

	return files;
}

static const char* strip_path_last(const char* path) {
	if (path == NULL || *path == '\0')
		return NULL;

	const char* lsep = NULL;
	const char* p = path;

	while (*p) {
		if (*p == '/' || *p == '\\')
			lsep = p;
		p++;
	}

	if (lsep == NULL)
		return path;

	return lsep + 1;
}

static char* concat_path_ext(const char* dir, const char* filename, const char* ext) {
	if (!dir || !filename || !ext || *dir == '\0' || *filename == '\0' || *ext == '\0')
		return NULL;

	size_t len = strlen(dir) + 1 + strlen(filename) + 1 + strlen(ext) + 1;
	char* result = (char*)malloc(len);
	if (result)
		snprintf(result, len, "%s/%s.%s", dir, filename, ext);
	return result;
}

static char* concat_ext(const char* filename, const char* ext) {
	if (!filename || !ext ||  *filename == '\0' || *ext == '\0')
		return NULL;

	size_t len = strlen(filename) + 1 + strlen(ext) + 1;
	char* result = (char*)malloc(len);
	if (result)
		snprintf(result, len, "%s.%s", filename, ext);
	return result;
}

static char* concat_paths(const char* dir, const char* extrapath) {
	if (!dir || !extrapath || *dir == '\0' || *extrapath == '\0')
		return NULL;

	size_t len = strlen(dir) + 1 + strlen(extrapath) + 1;
	char* result = (char*)malloc(len);
	if (result)
		snprintf(result, len, "%s/%s", dir, extrapath);
	return result;
}


static char* filename_without_extension(const char* path) {
	if (path == NULL || *path == '\0')
		return NULL;

	const char* lsep = NULL;
	const char* p = path;

	while (*p) {
		if (*p == '/' || *p == '\\')
			lsep = p;
		p++;
	}

	const char* fstr = (lsep != NULL) ? lsep + 1 : path;
	const char* lped = strrchr(fstr, '.');
	size_t flen = (lped != NULL) ? (size_t)(lped - fstr) : strlen(fstr);

	char* result = (char*)malloc(flen + 1);
	if (result == NULL)
		return NULL;

	strncpy(result, fstr, flen);
	result[flen] = '\0';

	return result;
}

static char* int_to_charptr(unsigned int i){
    int length = snprintf(NULL, 0, "%d", i);
    char* str = (char*)malloc(length + 1);
    snprintf(str, length + 1, "%d", i);
    return str;
}

// import data from file stream
static unsigned char* fs_import(FILE* file, size_t* len){
    const size_t chunk = 65536; // 64 KB
    size_t bufsize = chunk; // initial
    unsigned char* buf = (unsigned char*)malloc(bufsize);
    if(buf == NULL)
        return NULL;

    size_t tread;
    while(1){
        size_t bread = fread(buf + tread, 1, chunk, file);
        tread += bread;
        if(feof(file)){
            break;
        } else if(ferror(file)){
            free(buf);
            return NULL;
        }

        if(bread == chunk){
            bufsize += chunk;
            if((buf = (unsigned char*)realloc(buf, bufsize)) == NULL){
                free(buf);
                return NULL;
            }
        }
    }
    *len = tread;
    return buf;
}

static int stdout_export(unsigned char* dt, size_t len) {
#ifdef _WIN32
    if (_setmode(_fileno(stdout), _O_BINARY) == -1) {
        return 0;
    }
#endif

    if (fwrite(dt, 1, len, stdout) != len) {
        return 0;
    }
    fflush(stdout);
    return 1;
}
