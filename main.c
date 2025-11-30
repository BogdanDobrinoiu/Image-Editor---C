// Dobrinoiu Bogdan Mihai
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "image_editor.h"

int main(void)
{
	// Command and auxiliary input buffers
	char opt[20] = "";
	char s[100], f[5];

	// Image data (RGB and grayscale)
	rgb_t **rgb = NULL;
	int **mat = NULL;

	// Image properties
	int n = 0, m = 0, I = 0;

	// Selection coordinates
	int x1 = 0, x2 = n, y1 = 0, y2 = m;

	// State flags
	int ok = 0;      // Selection active
	int fl = 0;      // Image loaded flag
	int app = 0;    // Waiting for APPLY parameter
	int hist = 0;   // HISTOGRAM argument mode
	int ok2 = 0;

	int angle;

	// Main command processing loop
	while (strcmp(opt, "EXIT") != 0) {
		scanf("%s", opt);

		// LOAD command
		if (strcmp(opt, "LOAD") == 0) {
			if (app == 1) {
				app = 0;
				printf("Invalid command\n");
			}

			// Finalize pending histogram input
			if (ok2 == 1) {
				ok2 = 0;
				hist = 0;
				HISTOGRAM(&mat, &n, &m, s, f);
			} else if (hist == 1) {
				hist = 0;
				printf("Invalid command\n");
			}

			LOAD(&rgb, s, f, &n, &m, &I, &mat, &fl,
				 &x1, &x2, &y1, &y2);

		// SAVE command
		} else if (strcmp(opt, "SAVE") == 0) {
			if (fl == 1) {
				if (app == 1) {
					app = 0;
					printf("Invalid command\n");
				}

				if (ok2 == 1) {
					ok2 = 0;
					hist = 0;
					HISTOGRAM(&mat, &n, &m, s, f);
				} else if (hist == 1) {
					hist = 0;
					printf("Invalid command\n");
				}

				// Read output filename and save image
				getchar();
				fgets(s, sizeof(s), stdin);
				s[strcspn(s, "\n")] = '\0';
				SAVE(&rgb, &mat, &n, &m, s, f, &I);
			} else {
				getchar();
				fgets(s, sizeof(s), stdin);
				s[strcspn(s, "\n")] = '\0';
				printf("No image loaded\n");
			}

		// SELECT command
		} else if (strcmp(opt, "SELECT") == 0) {
			if (app == 1) {
				app = 0;
				printf("Invalid command\n");
			}

			if (ok2 == 1) {
				ok2 = 0;
				hist = 0;
				HISTOGRAM(&mat, &n, &m, s, f);
			} else if (hist == 1) {
				hist = 0;
				printf("Invalid command\n");
			}

			if (fl == 1) {
				// Parse selection coordinates
				getchar();
				fgets(s, sizeof(s), stdin);
				s[strcspn(s, "\n")] = '\0';
				SELECT(s, &x1, &x2, &y1, &y2, &n, &m);
				ok = 1;
			} else {
				getchar();
				fgets(s, sizeof(s), stdin);
				s[strcspn(s, "\n")] = '\0';
				printf("No image loaded\n");
			}

		// CROP command
		} else if (strcmp(opt, "CROP") == 0) {
			if (app == 1) {
				app = 0;
				printf("Invalid command\n");
			}

			if (ok2 == 1) {
				ok2 = 0;
				hist = 0;
				HISTOGRAM(&mat, &n, &m, s, f);
			} else if (hist == 1) {
				hist = 0;
				printf("Invalid command\n");
			}

			if (fl == 1) {
				if (ok == 1) {
					// Apply crop on current selection
					CROP(&rgb, &mat, &n, &m,
						 &x1, &x2, &y1, &y2, f);
					ok = 0;
				} else {
					printf("Image cropped\n");
				}
			} else {
				printf("No image loaded\n");
			}

		// APPLY command (expects filter parameter later)
		} else if (strcmp(opt, "APPLY") == 0) {
			if (ok2 == 1) {
				ok2 = 0;
				hist = 0;
				HISTOGRAM(&mat, &n, &m, s, f);
			} else if (hist == 1) {
				hist = 0;
				printf("Invalid command\n");
			}

			if (fl == 1) {
				app = 1;
			} else {
				printf("No image loaded\n");
			}

		// EQUALIZE command
		} else if (strcmp(opt, "EQUALIZE") == 0) {
			if (app == 1) {
				app = 0;
				printf("Invalid command\n");
			}

			if (ok2 == 1) {
				ok2 = 0;
				hist = 0;
				HISTOGRAM(&mat, &n, &m, s, f);
			} else if (hist == 1) {
				hist = 0;
				printf("Invalid command\n");
			}

			if (fl == 1) {
				EQUALIZE(&mat, &n, &m, f);
			} else {
				printf("No image loaded\n");
			}

		// HISTOGRAM command
		} else if (strcmp(opt, "HISTOGRAM") == 0) {
			if (app == 1) {
				app = 0;
				printf("Invalid command\n");
			} else if (!fl) {
				printf("No image loaded\n");
			} else {
				char additional_args[100];
				hist = 1;

				fgets(additional_args, sizeof(additional_args), stdin);
				additional_args[strcspn(additional_args, "\n")] = '\0';

				memset(s, 0, sizeof(s));

				if (strlen(additional_args) == 0) {
					printf("Invalid command\n");
					hist = 0;
				} else {
					strcpy(s, additional_args);
					HISTOGRAM(&mat, &n, &m, s, f);
					hist = 0;
				}
			}

		// ROTATE command
		} else if (strcmp(opt, "ROTATE") == 0) {
			if (app == 1) {
				app = 0;
				printf("Invalid command\n");
			}

			if (ok2 == 1) {
				ok2 = 0;
				hist = 0;
				HISTOGRAM(&mat, &n, &m, s, f);
			} else if (hist == 1) {
				hist = 0;
				printf("Invalid command\n");
			}

			if (fl == 1) {
				scanf("%d", &angle);
				ROTATE(&rgb, &mat, &n, &m, f,
					   &angle, &x1, &x2, &y1, &y2);
			} else {
				printf("No image loaded\n");
				getchar();
				fgets(s, sizeof(s), stdin);
				s[strcspn(s, "\n")] = '\0';
			}

		// APPLY filter execution
		} else if (app == 1) {
			if (strcmp(opt, "EXIT") == 0) {
				printf("Invalid command\n");
				break;
			}

			app = 0;
			strcpy(s, opt);
			APPLY(&rgb, &n, &m, f, s,
				  &x1, &x2, &y1, &y2);

		// HISTOGRAM multi-token collection
		} else if (hist == 1) {
			if (strcmp(opt, "EXIT") == 0) {
				printf("Invalid command\n");
				break;
			}

			if (ok2 == 0) {
				strcpy(s, opt);
				ok2 = 1;
			} else {
				strcat(s, " ");
				strcat(s, opt);
			}

		// Invalid command case
		} else if (strcmp(opt, "EXIT") != 0) {
			if (fl == 1) {
				printf("Invalid command\n");
				getchar();
				fgets(s, sizeof(s), stdin);
				s[strcspn(s, "\n")] = '\0';
			}
		}
	}

	// Cleanup before exit
	if (fl == 0)
		printf("No image loaded\n");

	free_mat(&mat, &n);
	free_rgb(&rgb, &n);

	return 0;
}