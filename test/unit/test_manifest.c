/*
 *   Software Updater - client side
 *
 *      Copyright (c) 2019-2020 Intel Corporation.
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, version 2 or later of the License.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>

#include "swupd_lib/manifest.h"
#include "swupd.h"
#include "test_helper.h"

static void validate_file(struct list *files, const char *filename, int version, int hash, int is_dir, int is_file, int is_link, int is_deleted, int is_ghosted, int is_manifest, int is_config, int is_state, int is_boot, int is_experimental)
{
	struct list *list;
	char hash_str[SWUPD_HASH_LEN];
	snprintf(hash_str, SWUPD_HASH_LEN, "%064d", hash);

	for (list = files; list; list = list->next) {
		struct file *file = list->data;
		if (str_cmp(filename, file->filename) == 0) {
			check(str_cmp(file->hash, hash_str) == 0);
			check(file->is_dir == is_dir);
			check(file->is_file == is_file);
			check(file->is_link == is_link);
			check(file->is_deleted == is_deleted);
			check(file->is_ghosted == is_ghosted);
			check(file->is_manifest == is_manifest);
			check(file->is_config == is_config);
			check(file->is_state == is_state);
			check(file->is_boot == is_boot);
			check(file->is_experimental == is_experimental);

			check(file->last_change == version);

			return;
		}
	}

	printf("File not found (%s:%d)\n", __FILE__, __LINE__);
	exit(EXIT_FAILURE);
}

struct manifest *manifest_parse_test(const char *component, const char *dir, const char *filename, bool header_only)
{
	struct manifest *m;
	char *full_path;

	full_path = sys_path_join("%s/%s", dir, filename);
	m = manifest_parse(component, full_path, header_only);
	FREE(full_path);

	return m;
}

static void test_manifest_parse()
{
	struct manifest *manifest;
	char *dir;

	dir = sys_dirname(__FILE__);
	// parsing a missing file should fail
	manifest = manifest_parse_test("test", dir, "test/unit/missing", false);
	check(manifest == NULL);

	// Check if parser can parse the manifest header even if files are invalid
	manifest = manifest_parse_test("test", dir, "data/mom1", true);
	check(manifest != NULL);
	check(manifest->manifest_version == 1);
	check(manifest->version == 30);
	check(manifest->filecount == 123);
	check(manifest->contentsize == 789);
	manifest_free(manifest);

	// Manifest parser should fail on incorrect file list
	manifest = manifest_parse_test("test", dir, "data/mom1", false);
	check(manifest == NULL);

	// Check if parser can parse all different flags supported for the file list
	manifest = manifest_parse_test("test", dir, "data/mom2", false);
	check(manifest != NULL);
	check(list_len(manifest->files) == 13);
	check(list_len(manifest->manifests) == 1);

	validate_file(manifest->manifests, "m1", 10, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0);

	validate_file(manifest->files, "f1", 20, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
	validate_file(manifest->files, "f2", 30, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	validate_file(manifest->files, "f3", 30, 3, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
	validate_file(manifest->files, "f4", 30, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	validate_file(manifest->files, "f5", 30, 5, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0);
	validate_file(manifest->files, "f6", 30, 6, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0);
	validate_file(manifest->files, "f7", 30, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
	validate_file(manifest->files, "f8", 30, 8, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0);
	validate_file(manifest->files, "f9", 30, 9, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
	validate_file(manifest->files, "f10", 30, 10, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
	validate_file(manifest->files, "f11", 30, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	validate_file(manifest->files, "f12", 30, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	validate_file(manifest->files, "f13", 30, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	manifest_free(manifest);

	// Missing MANIFEST keyword on header
	manifest = manifest_parse_test("test", dir, "data/mom_invalid1", false);
	check(manifest == NULL);

	// Invalid header format
	manifest = manifest_parse_test("test", dir, "data/mom_invalid2", false);
	check(manifest == NULL);

	FREE(dir);
}

int main() {
	test_manifest_parse();

	return 0;
}
