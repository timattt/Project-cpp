#include <bits/stdc++.h>
#include <windows.h>

/*
 *	Author timattt
 */

int tCount(const char *line, char symb);
int tStrlen(const char *str);
int tGetFileSize(const char *name);
const char* recreateBuffer(const char *raw, int &len, const char *un);
const char* tReadMappedBuffer(const char *name, int &buffer_size);
void tReverse(char *a, char *b);
void tSwap(char *&a, char *&b);
bool comparator(const char *a_, const char *b_);
void tQsort(char **arr, int a, int b);
void tSortLines(char **lines, int quant);
char** tReindex(const char *buf, int length, int &lines_quant);

//!Gives quantity of symbols in this text.
int tCount(const char *line, char symb) {
	if (!line) {
		return 0;
	}

	int result = 0;
	for (int i = 0; line[i]; i++) {
		if (line[i] == symb) {
			result++;
		}
	}

	return result;
}

//!Gives quantity of chars in this char array.
int tStrlen(const char *str) {
	assert(str != NULL);
	const char *s = str;
	for (; *s; s++)
		;
	return s - str;
}

//!Gives file size in bytes.
int tGetFileSize(const char *name) {
	WIN32_FIND_DATA data = { };
	FindFirstFileA(name, &data);
	return data.nFileSizeLow;
}

//!Converts char array with given length into array of lines.
//!Splits this char array by [\n] and return array of pointers.
//!@param text -array of chars to parse.
//!@param length - length of this text.
//!@param lines_quant - here will be stored quantity of lines.
//!@return array of lines.
char** tReindex(const char *buf, int length, int &lines_quant) {
	assert(buf != NULL && length >= 0);

	lines_quant = tCount(buf, '\n');

	char **result = (char**) calloc(lines_quant, sizeof(char*));

	for (char *symb = (char*) buf; symb != buf + length; symb++) {
		if ((*symb) == '\n') {
			if (!(*result)) {
				(*result) = symb;
			}
			result++;
		} else if ((*result) == NULL) {
			(*result) = symb;
		}
	}

	result -= lines_quant;

	return result;
}

//!This function distributes lines into by groups as rhyme_map says.
//!
//!
//!
//!
//!@Note lines will be freed.
char*** tRepackByRhyme(char **&lines, int lines_quant, const int *rhyme_map,
		const int *rhyme_group_sizes, int rhyme_map_size, int total_groups,
		int &total_blocks) {
	assert(lines != NULL && rhyme_map != NULL && rhyme_group_sizes != NULL);
	char ***groups = (char***) calloc(total_groups, sizeof(char**));

	total_blocks = (int) (lines_quant / rhyme_map_size);

	for (int group_i = 0; group_i < total_groups; group_i++) {
		groups[group_i] = (char**) calloc(
				rhyme_group_sizes[group_i] * total_blocks, sizeof(char*));
	}

	for (int i = 0; i < rhyme_map_size * total_blocks; i += rhyme_map_size) {
		int loc_index = 0;
		for (int group_i = 0; group_i < total_groups; group_i++) {
			for (int group_element_counter = 0;
					group_element_counter < rhyme_group_sizes[group_i];
					group_element_counter++, loc_index++) {
				(*groups[group_i]) = lines[i + rhyme_map[loc_index]];
				groups[group_i]++;
			}
		}
	}

	for (int g = 0; g < total_groups; g++) {
		groups[g] -= (total_blocks * rhyme_group_sizes[g]);
	}

	free(lines);
	lines = NULL;

	return groups;
}

/*!
 *	Shuffles pair in groups.
 */
void random_shuffle_group(char **group, int size) {
	assert(group != NULL);
	for (int i = 0; i < size * 4; i++) {
		int rand_i1 = 2 * (rand() % (size / 2));
		int rand_i2 = 2 * (rand() % (size / 2));

		tSwap(group[rand_i1], group[rand_i2]);
		tSwap(group[rand_i1 + 1], group[rand_i2 + 1]);
	}
}

/*!
 *	This is just a wrapper to randomize every line pair in every group.
 */
void randomize_groups(char ***groups, int total_groups, int total_blocks,
		const int *group_sizes) {
	assert(groups != NULL && group_sizes != NULL);
	for (int g = 0; g < total_groups; g++) {
		random_shuffle_group(groups[g], group_sizes[g] * total_blocks);
	}
}

/*!
 * This function makes normal lines list from groups. Everything is repacked in order to rhyme map.
 */
const char** repackGroups(char ***&groups, const int total_groups,
		const int total_blocks, const int *rhyme_map,
		const int *rhyme_group_sizes, const int rhyme_map_size) {
	assert(groups != NULL && rhyme_map != NULL && rhyme_group_sizes != NULL);
	const char **result = (const char**) calloc(rhyme_map_size * total_blocks,
			sizeof(char*));

	for (int block_i = 0, result_i = 0; block_i < total_blocks;
			block_i++, result_i += rhyme_map_size) {

		for (int group_i = 0, local_i = 0; group_i < total_groups; group_i++) {
			for (int y = 0; y < rhyme_group_sizes[group_i]; y++, local_i++) {
				result[block_i * rhyme_map_size + rhyme_map[local_i]] =
						(*groups[group_i]);
				groups[group_i]++;
			}
		}
	}

	free(groups);
	groups = NULL;

	return result;
}

//!This function recreates buffer of chars but removes unused symbols.
//!@param raw - array to recreate.
//!@param len - length of this array.
//!@param un - array of symbols to remove.
//!@returns newly create array raw_ but without symbols that are in un.
const char* recreateBuffer(const char *raw, int &len, const char *un) {
	assert(raw != NULL && len >= 0 && un != NULL);

	int symbolsToCleanup = 0;
	for (const char *s = raw; s != raw + len; s++) {
		for (const char *t = un; *t; t++) {
			if ((*s) == (*t)) {
				symbolsToCleanup++;
				break;
			}
		}
	}

	int new_len = len - symbolsToCleanup + 1;
	char *rec = (char*) calloc(new_len, sizeof(const char*));
	for (const char *s = raw; s != raw + len; s++) {
		bool unused = false;
		for (const char *t = un; *t; t++) {
			if ((*s) == (*t)) {
				unused = true;
				break;
			}
		}
		if (!unused) {
			(*rec) = (*s);
			rec++;
		}
	}

	(*rec) = '\n';
	rec++;

	len = new_len;
	rec -= new_len;

	return rec;
}

//!This function reads all chars from file using mapping.
//!@param name - name of the file.
//!@param lines_quant - there will be stored quantity of lines.
//!@returns array of lines.
const char* tReadMappedBuffer(const char *name, int &buffer_size) {
	assert(name != NULL && buffer_size > 0);

	HANDLE file = CreateFileA(name, GENERIC_READ,
	FILE_SHARE_READ, NULL,
	OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	assert(file != INVALID_HANDLE_VALUE);

	HANDLE handle = CreateFileMappingA(file, NULL, PAGE_READONLY, 0, 0, NULL);

	assert(handle != INVALID_HANDLE_VALUE);

	LPVOID raw_buf = MapViewOfFile(handle, FILE_MAP_READ, 0, 0, 0);

	assert(raw_buf != NULL);

	const char *buf = (const char*) raw_buf;

	buffer_size = tStrlen(buf);

	char unused[] = { '\r' };
	const char *rec_buf = recreateBuffer(buf, buffer_size, unused);

	UnmapViewOfFile(buf);
	CloseHandle(handle);
	CloseHandle(file);

	return rec_buf;
}

//!Reverses everything that is stands between a and b.
void tReverse(char *a, char *b) {
	assert(a != NULL && b != NULL);
	char *aa = a;
	char *bb = b;
	while (aa < bb) {
		char tmp = *aa;
		(*aa) = (*bb);
		(*bb) = tmp;
		(aa)++;
		(bb)--;
	}
}

//!This function reverses all lines.
void tReverseAll(char **lines, int total) {
	assert(lines != NULL && total >= 0);
	for (char **line = lines; line != lines + total; line++) {
		int size = 0;
		while ((*line)[size] != '\n') {
			size++;
		}
		tReverse(*line, *line + size - 1);
	}
}

//!swaps two strings.
void tSwap(char *&a, char *&b) {
	assert(a != NULL && b != NULL);
	char *tmp = a;
	a = b;
	b = tmp;
}

//!This function compares two strings.
//!@returns true if a < b else false
bool comparator(const char *a_, const char *b_) {
	assert(a_ != NULL && b_ != NULL);

	const char *aa = a_;
	const char *bb = b_;

	int len_a = 0;
	while (aa[len_a] != '\n') {
		len_a++;
	}
	int len_b = 0;
	while (bb[len_b] != '\n') {
		len_b++;
	}

	int a = len_a - 1;
	int b = len_b - 1;

	while (a != -1 && b != -1) {
		int sa = tolower(aa[a]);
		int sb = tolower(bb[b]);

		if (std::isalpha(sa) != 0 && std::isalpha(sb) != 0) {
			if (sa != sb) {
				return sa < sb;
			} else {
				a--;
				b--;
			}
			continue;
		}
		if (std::isalpha(sa) == 0) {
			a--;
		}
		if (std::isalpha(sb) == 0) {
			b--;
		}
	}

	return 0;
}

//!Sorts using quick sort algorithm array of lines.
void tQsort(char **arr, int a, int b) {
	assert(arr != NULL && a <= b);
	int left = a, right = b;
	char *piv = arr[(left + right) / 2];
	while (left <= right) {
		//arr[l] < piv
		while (comparator(arr[left], piv)) {
			left++;
		}
		//arr[right] > piv
		while (comparator(piv, arr[right])) {
			right--;
		}
		if (left <= right) {
			tSwap(arr[left++], arr[right--]);
		}
	}
	if (a < right)
		tQsort(arr, a, right);
	if (b > left)
		tQsort(arr, left, b);
}

//!This function sorts given lines by alphabet.
//!@param
void tSortLines(char **lines, int quant) {
	assert(lines != NULL);
	tQsort(lines, 0, quant - 1);
}

//!Writes line into given file until stop symbol is appear.
void tWriteLineToFile(const char *line, char stop_symb, FILE *f) {
	assert(line != NULL);
	const char *l = line;
	while ((*l) != stop_symb) {
		fputc(*l, f);
		l++;
	}
	fputc('\n', f);
}

//-----------------------------------------------------------------------
//!This function saves all lines into given file.
//!@param lines - array of lines to be written.
//!@param quant - quantity of lines
//!@param fileName - name of file where data will be saved.
//-----------------------------------------------------------------------
void tWriteToFile(const char **lines, int quant, const char *fileName) {
	assert(lines != NULL && quant >= 0 && fileName != NULL);

	FILE *f = fopen(fileName, "wb");

	for (int i = 0; i < quant; i++) {
		const char *line = lines[i];
		if (!line) {
			tWriteLineToFile("\0", '\0', f);
			continue;
		}
		tWriteLineToFile(line, '\n', f);
	}

	fclose(f);
}

const int rhyme_map_size = 15;
const int rhyme_map[rhyme_map_size] = { 1, 3, 2, 4, 10, 11, 5, 6, 9, 12, 7, 8,
		13, 14 };
const int rhyme_group_sizes[] = { 2, 4, 4, 4 };
const int total_groups = 4;

int main() {
	int lines_quantity = 0;
	int buffer_size = 0;
	int total_rhyme_blocks = 0;

	const char *buffer = tReadMappedBuffer("RAJ.txt", buffer_size);

	char **lines = tReindex(buffer, buffer_size, lines_quantity);

	char ***groups = tRepackByRhyme(lines, lines_quantity, rhyme_map,
			rhyme_group_sizes, rhyme_map_size, total_groups,
			total_rhyme_blocks);

	randomize_groups(groups, total_groups, total_rhyme_blocks,
			rhyme_group_sizes);

	const char **lines_raved = repackGroups(groups, total_groups,
			total_rhyme_blocks, rhyme_map, rhyme_group_sizes, rhyme_map_size);

	tWriteToFile(lines_raved, lines_quantity, "OUT.txt");

	free(lines_raved);

	return 0;
}
