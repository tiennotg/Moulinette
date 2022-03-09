/*
 *  process.h
 *
 *  This file is part of Moulinette.
 *
 *  Moulinette is free software: you can redistribute it and/or modify it under the terms of
 *  the GNU General Public License as published by the Free Software Foundation, either
 *  version 3 of the License, or (at your option) any later version.
 * 
 *  Moulinette is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE. See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with Moulinette.
 *  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef PROCESS_H
#define PROCESS_H

#include "filelist.h"
#include "fileitem.h"

#define PROCESS_CMD "bin\\ffmpeg.exe"
#define PROCESS_ARGS1 "-hide_banner -y -i"
#define PROCESS_ARGS2 "-ar 48000 -filter:a \"loudnorm=i=-9:dual_mono=true:tp=0:lra=7,alimiter=limit=0dB:level_out=-7dB\""
#define PROCESS_OUTPUT_DIR "moulinette3\\"
#define PROCESS_END_CMD "explorer "
#define PROCESS_RUNNING_WARNING "Une conversion est en cours, êtes-vous sûr de vouloir quitter ?"

static char *PROCESS_RUN_ERROR_STR[] = {"OK","erreur générale","impossible de créer le pipe","impossible de lancer ffmpeg","","","","","","","",
	"erreur de copie du .DBX", "ffmpeg a retourné une erreur.", "impossible de créer le dossier de sortie."};

int process_start(GtkFileList *file_list);
int process_run(unsigned long *ret_code);
int process_file(char *filename);
void process_stop(void);
void process_empty_dir(void);
char *process_get_dest_dir(void);

#endif