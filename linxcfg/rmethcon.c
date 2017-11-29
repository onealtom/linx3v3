/*
 * Copyright (c) 2006-2007, Enea Software AB
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * Neither the name of Enea Software AB nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * File: rmethcon.c - Command to remove a Linx Ethernet connection.
 */
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linxcfg.h>
#include <ethcm_db_ioctl.h>

struct option long_opts[] = {
        {"help", 0, NULL, (int)'h'},
        {NULL, 0, NULL, 0}
};

static char *usage =
"rmethcon [OPTIONS] <connection>...\n"
"Remove Linx Ethernet connections.\n"
"OPTIONS:\n"
"    -h, --help\n"
"        Print this text.\n"
"Examples:\n"
"    Remove the Linx Ethernet connection ETHCON_A.\n"
"    alt1. rmethcon ETHCON_A\n"
"    alt2. rmethcon ethcm/ETHCON_A\n";

int main(int argc, char *argv[])
{
        char **con, *key, *fmt;
        int c, status;

        for (c = 0; c != -1;) {
                c = getopt_long(argc, argv, "h", long_opts, NULL);
                switch (c) {
                case 'h':
                        printf("Usage: %s\n", usage);
                        return 0;
                case '?':
                        return 1;
                }
        }

        if (argv[optind] == NULL) {
                printf("rmethcon: connection missing\n"
                       "Try 'rmethcon --help' for more information\n");
                return 1;
        }

        for (con = &argv[optind]; *con != NULL; con++) {
                if (strncmp(*con, DB_KEY_ETHCM "/", strlen(DB_KEY_ETHCM)+1)==0)
                        fmt = "%s";
                else
                        fmt = DB_KEY_ETHCM "/%s";
                key = malloc(1024);
                if (key == NULL) {
                        printf("rmethcon: couldn't allocate memory\n");
                        return 1;
                }
                if (snprintf(key, 1024, fmt, *con) < 0) {
                        printf("rmethcon: snprintf failed: %s\n",
                               strerror(errno));
                        free(key);
                        return 1;
                }
                status = linx_remove_connection(key);
                if (status != 0) {
                        printf("rmethcon: couldn't remove connection '%s': ",
			       key);
                        if (status == ENOENT)
                                printf("No such connection\n");
                        else
                                printf("%s\n", strerror(status));
                        free(key);
                        return 1;
                }
                free(key);
        }
        return 0;
}
