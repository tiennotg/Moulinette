/*
 *  moulinette3.h
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

// Protection contre les inclusions multiples
#ifndef MOULINETTE3_H
#define MOULINETTE3_H

#define VBOX_TREE_PADDING 0
#define VBOX_BUTTONBAR_PADDING 5

#define FILE_EXT_DBX "DBX"
#define FILE_EXT_WAV "WAV"
#define FILE_EXT_MP3 "MP3"
#define FILE_EXT_MP2 "MP2"

#define MAIN_WINDOW_TITLE "Moulinette v3.1"
#define MAIN_WINDOW_WIDTH 600
#define MAIN_WINDOW_HEIGHT 400

#define ERROR_MSG_FFMPEG "FFmpeg n'est pas correctement installé sur votre poste, et est nécessaire pour le bon fonctionnement du programme."
#define ERROR_MSG_PROJECT "Les projets ne peuvent pas être passés à la moulinette !"
#define ERROR_MSG_IMPORT "Erreur lors de l'importation : type de fichier non pris en charge."
#define PROCESS_END_MSG "La conversion est terminée. Sélectionnez l'ensemble des fichiers dans la fenêtre qui s'ouvre, et glissez-les dans Digas.\n\n\
Une fois l'import terminé, vous pouvez cliquer sur Terminer."
#define PROCESS_ERROR_MSG "Une erreur est survenue lors de la conversion : "
#define PROCESS_END_BTN "Terminer"

#endif