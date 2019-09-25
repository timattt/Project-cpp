#ifndef T_FILE_HANDLER
#define T_FILE_HANDLER

#include <windows.h>
#include <bits/stdc++.h>
#include "tUtilities.h"

using namespace tUtilities;

namespace tFileHandler {

class tFile {

	// Name
	char *name;

	// File handle
	HANDLE file_handle;

	// Mapping
	HANDLE file_mapping_handle;
	char *mapped_buffer;

	tFile(const char *name_) :
			mapped_buffer(NULL), file_mapping_handle(), name(name_) {
		file_handle = CreateFileA(name, GENERIC_READ,
		FILE_SHARE_READ, NULL,
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (file_handle == INVALID_HANDLE_VALUE) {
			tThrowException("Invalid file handle!");
		}

	}

	~tFile() {
		tCloseMapping();
		tCloseFile();
	}

	void tMapFile() {
		file_mapping_handle = CreateFileMappingA(file_handle, NULL,
		PAGE_READONLY, 0, 0, NULL);
		if (file_mapping_handle == INVALID_HANDLE_VALUE) {
			tThrowException("Invalid mapping handle!");
		}

		mapped_buffer = (char*) MapViewOfFile(file_mapping_handle,
		FILE_MAP_READ, 0, 0, 0);
		if (mapped_buffer == NULL) {
			tThrowException("Mapped buffer is NULL!");
		}
	}

	void tCloseMapping() {
		if (mapped_buffer != NULL) {
			UnmapViewOfFile(mapped_buffer);
		}
		CloseHandle(file_mapping_handle);
	}

	void tCloseFile() {
		CloseHandle(file_handle);
	}
};

}

#endif
