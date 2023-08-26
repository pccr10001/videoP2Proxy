/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * Copyright (C) 2018 Miguel Angel Nubla <miguelangel.nubla@gmail.com>
 */

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "config.h"
#include "common.h"
#include "client.h"
#include "rtsp.h"

void print_usage() {
    printf(
                "Usage: videop2proxy --uid UID --username USERNAME --password PASSWORD [...] \n"
                "\n"
                "Options:\n"
                "  --uid UID             [Required] TUTK P2P ID.\n"
                "  --username USERNAME   [Required] TUTK Camera username Default: admin.\n"
                "  --password PASSWORD   [Required] TUTK Canera password.\n"
                        "\n"
                "Modes:\n"
#ifdef ENABLE_RTSP
                        "  --rtsp PORT         Enable RTSP server.\n"
#endif
                "  --stdout            Enable output to stdout.\n"
    );
}

int tryConnect(char* uid, char* username, char* password) {
        return clientRun(uid, username, password);
}

int main(int argc, char *argv[]) {
    char *username = "", *uid = "", *password = "";
        MODE_RTSP = -1, MODE_STDOUT = -1;

    static struct option long_options[] = {
        {"uid",        required_argument, 0,  'i' },
        {"username",   required_argument, 0,  'u' },
        {"password",   required_argument, 0,  'p' },
                #ifdef ENABLE_RTSP
                {"rtsp",   required_argument, 0,  'r' },
                #endif
                {"stdout", no_argument,       0,  's' },
        {0,        0,                 0,  0   }
    };

        int opt= 0;
    int long_index =0;
    while ((opt = getopt_long(argc, argv,"i:u:p:rs",
                   long_options, &long_index )) != -1) {
        switch (opt) {
             case 'i' : uid = optarg;
                 break;
             case 'u' : username = optarg;
                 break;
             case 'p' : password = optarg;
                 break;
             case 'r' : MODE_RTSP = atoi(optarg);
                 break;
             case 's' : MODE_STDOUT = 1;
                 break;
             default: print_usage();
                 exit(EXIT_FAILURE);
        }
    }
    if (MODE_RTSP < 0 && MODE_STDOUT < 0) {
        printf("ERROR: You must specify at least one mode.\n");
        print_usage();
        exit(EXIT_FAILURE);
    }


        #ifdef ENABLE_RTSP
        if (MODE_RTSP >= 0)
        {
                char template[] = "/tmp/videop2proxy.XXXXXX";
                char* tmpDir = mkdtemp(template);
                char* tmpFile = "/fifo";

                MODE_RTSP_FIFO_FILE = malloc(strlen(tmpDir) + strlen(tmpDir) + 1);
                strcat(MODE_RTSP_FIFO_FILE, tmpDir);
                strcat(MODE_RTSP_FIFO_FILE, tmpFile);

                mkfifo(MODE_RTSP_FIFO_FILE, 0600);

                pthread_t ThreadRTSP = 0;
                int ret;
                if ((ret=pthread_create(&ThreadRTSP, NULL, &min, NULL)))
                {
                        DPRINTF("Create RTSP thread failed\n");
                        return 1;
                }
                MODE_RTSP_FIFO = open(MODE_RTSP_FIFO_FILE, O_WRONLY);
        }
        #endif

        DPRINTF("Starting proxy...\n");
        int delay = 10;
    while (1)
        {
                tryConnect(uid, username, password);
                DPRINTF("Error, waiting %d seconds and trying again.\n", delay);
                sleep(delay);
        }
}
