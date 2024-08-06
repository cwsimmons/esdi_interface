
/* 

    Copyright 2024 Christopher Simmons

  This program is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation, either version 2 of the License, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along
  with this program. If not, see <https://www.gnu.org/licenses/>.

*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>

#include "crc.h"
#include "types.h"
#include "serial_command.h"
#include "datapath.h"
#include "drive.h"
#include "util.h"
#include "controller.h"

#include "ibm_5364.h"
#include "ibm_ps2.h"
#include "ibm_rt_enhanced.h"
#include "ultrastor_12f.h"

void shutdown() {
    printf("Shutting down.\n");
    set_drive_select(0);
    datapath_stop();
}

bool stop = false;

void ctrlc(int arg) {

    stop = true;
}

int main(int argc, char** argv)
{
    int c;
    int option_index = 0;

    bool verbose = false;

    int drive_select = -1;
    int sector_mode = -1;
    int cylinders = -1;
    int heads = -1;
    int sectors = -1;
    int controller = -1;
    int starting_cylinder = 0;
    int max_attempts = 5;

    char extract_data_filename[FILENAME_MAX] = "";
    char data_crc_analysis_prefix[FILENAME_MAX] = "";

    char* sector_mode_options[2] = {"hard", "soft"};
    
    static struct option long_options[] = {
        {"drive",           required_argument, 0, 'd'},
        {"sector-mode",     required_argument, 0, 'm'},
        {"cylinders",       required_argument, 0, 'c'},
        {"heads",           required_argument, 0, 'h'},
        {"sectors",         required_argument, 0, 's'},
        {"verbose",         no_argument,       0,  0 },
        {"extracted-data",  required_argument, 0, 'e'},
        {"controller",      required_argument, 0, 'C'},
        {"start-cyl",       required_argument, 0, 'S'},
        {"max-attempts",    required_argument, 0, 'r'},
        {"data-crc-analysis", required_argument, 0, 1},
        {0,                 0,                 0,  0 }
    };

    static struct esdi_controller* controllers[] = {
        &ibm_5364,
        &ibm_ps2,
        &ibm_rt_enhanced,
        &ultrastor_12f,
        NULL
    };

    signal(SIGINT, ctrlc);

    while ((c = getopt_long(argc, argv, "d:m:c:h:s:e:C:S:r:", long_options, &option_index)) != -1) {

        switch (c) {
            case 0:
                verbose = true;
                break;

            case 'd':
                sscanf(optarg, "%i", &drive_select);
                break;

            case 'm':
                for (int i = 0; i < 2; i++) {
                    if (strcasecmp(sector_mode_options[i], optarg) == 0)
                        sector_mode = i;
                }
                break;

            case 'c':
                sscanf(optarg, "%i", &cylinders);
                break;

            case 'h':
                sscanf(optarg, "%i", &heads);
                break;

            case 's':
                sscanf(optarg, "%i", &sectors);
                break;

            case 'e':
                strcpy(extract_data_filename, optarg);
                break;

            case 'C':
                for (int i = 0; controllers[i] != NULL; i++) {
                    if (strcasecmp(controllers[i]->name, optarg) == 0)
                        controller = i;
                }
                break;

            case 'S':
                sscanf(optarg, "%i", &starting_cylinder);
                break;

            case 'r':
                sscanf(optarg, "%i", &max_attempts);
                break;

            case 1:
                strcpy(data_crc_analysis_prefix, optarg);
                break;
                
            case '?':
                break;

            default:
                break;
        }
    }

    if (drive_select == -1) {
        printf("Please specify the drive select number (-d)\n");
        exit(EXIT_FAILURE);
    }

    if (controller == -1) {
        printf("Unknown controller!\n");

        printf("Controller options are:\n");
        for (int i = 0; controllers[i] != NULL; i++) {
            printf("%s\n", controllers[i]->name);
        }

        exit(EXIT_FAILURE);
    }

    if (strlen(extract_data_filename) == 0) {
        printf("Filename not provided\n");
        exit(EXIT_FAILURE);
    }

    FILE* extract_fd = fopen(extract_data_filename, "wb");

    if (extract_fd == NULL) {
        printf("Failed to open extract data file for writing\n");
        exit(EXIT_FAILURE);
    }

    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        printf("Failed to open /dev/mem\n");
        exit(EXIT_FAILURE);
    }

    volatile uint32_t* mem_base = (uint32_t*) mmap(NULL, 0x80000, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0xa0000000);
    if (mem_base == MAP_FAILED) {
        printf("Failed to mmap\n");
        exit(EXIT_FAILURE);
    }

    serial_command_base = mem_base;
    sector_timer_base = &mem_base[(0x1000 >> 2)];
    datapath_base = &mem_base[0x2000 >> 2];
    bram_base = (uint8_t*) &mem_base[0x40000 >> 2];
    dma_base = &mem_base[0x10000 >> 2];

    atexit(shutdown);


    set_drive_select(drive_select);
    drive_reset();


    struct drive_parameters drive_params;
    if (serial_query_drive_parameters(&drive_params)) {
        printf("Failed to query drive parameters\n");
        exit(EXIT_FAILURE);
    }


    if (sector_mode != -1)
        drive_params.is_soft_sectored = (sector_mode == 1) ? true : false;
    if (cylinders != -1)
        drive_params.cylinders = cylinders;
    if (heads != -1)
        drive_params.heads = heads;

    if (sectors == -1) {
        if (!drive_params.is_soft_sectored)
            drive_params.sectors = drive_params.sectors_hard;
        else
            drive_params.sectors = -1;
    } else {
        drive_params.sectors = sectors;
    }

    if (max_attempts < 1)
        max_attempts = 1;

    printf("Drive Parameters:\n");
    printf("    Sectoring Mode: %s\n", sector_mode_options[drive_params.is_soft_sectored == 1]);
    printf("    Cylinders:      %d\n", drive_params.cylinders);
    printf("    Heads:          %d\n", drive_params.heads);
    if (drive_params.sectors == -1)
        printf("    Sectors:        TBD\n");
    else
        printf("    Sectors:        %d\n", drive_params.sectors);

    struct esdi_controller* controller_params = controllers[controller];

    int allocated_sectors = (drive_params.sectors > 0) ? drive_params.sectors : 128;

    int physical_sectors[128];
    int expected_lbas[128];
    int processed_lbas[128];

    int num_processed_lbas;

    struct raw_sector* raw_sectors = (struct raw_sector*) malloc(sizeof(struct raw_sector) * allocated_sectors);
    struct processed_sector* processed_sectors = (struct processed_sector*) malloc(sizeof(struct processed_sector) * allocated_sectors);

    if (raw_sectors == NULL)
        printf("Out of memory!\n");

    for (int i = 0; i < allocated_sectors; i++) {
        raw_sectors[i].address_area = (uint8_t*) malloc(sizeof(uint8_t) * controller_params->addr_area_length);
        if (raw_sectors[i].address_area == NULL) printf("Out of memory!\n");

        raw_sectors[i].data_area = (uint8_t*) malloc(sizeof(uint8_t) * controller_params->data_area_length);
        if (raw_sectors[i].data_area == NULL) printf("Out of memory!\n");

        processed_sectors[i].data = (uint8_t*) malloc(sizeof(uint8_t) * controller_params->data_area_length);
        if (processed_sectors[i].data == NULL) printf("Out of memory!\n");
    }

    if (drive_params.sectors == -1) {
        printf("Discovering number of sectors not currently supported.\n");
        exit(EXIT_FAILURE);
    }

    // I thought there might be time when we don't want to read all
    // of the sectors on the track. Could probably be removed.
    for (int i = 0; i < drive_params.sectors; i++) {
        physical_sectors[i] = i;
    }

    datapath_configure(&drive_params, controller_params);
    datapath_start();

    flush_fifo();
    reset_dma();

    for (int j = starting_cylinder; (j < drive_params.cylinders) && !stop; j++) {

        drive_seek(j);
        usleep(100000);

        printf("At cyl %d\n", j);

        for (int k = 0; (k < drive_params.heads) && !stop; k++) {

            set_head_select(k);
            usleep(10000);


            // Get the LBAs that we expect on this track
            int num_expected_lbas = controller_params->get_expected_lbas(
                &drive_params,
                j,
                k,
                expected_lbas    
            );

            num_processed_lbas = 0;
            int attempt = 0;

            // Keep trying until we have all of the LBAs we expect
            while ((attempt < max_attempts) && (num_processed_lbas < num_expected_lbas) && !stop) {

                if (attempt > 0) {
                    int rem = num_expected_lbas - num_processed_lbas;
                    printf(
                        "Starting Attempt #%d on [%d,%d] to get %d sector%s\n",
                        attempt + 1, j, k, rem, (rem > 1) ? "s" : ""
                    );
                }

                // Read all sectors on the track
                read_track_sg(drive_params.sectors, physical_sectors, raw_sectors);

                // Process the sectors
                for (int i = 0; i < drive_params.sectors; i++) {
                    
                    raw_sectors[i].cylinder = j;
                    raw_sectors[i].head = k;
                    raw_sectors[i].physical_sector = physical_sectors[i];

                    // Move on if there was a problem reading this sector
                    if (raw_sectors[i].status) {
                        // If we didn't even manage to get the address area then we got nothing, move on...
                        if (!raw_sectors[i].address_read_ok)
                            continue;
                    }

                    // The point of this block is to aid in determining the data crc.
                    // Since I don't know the if the sector is good,
                    // I just have to write the first one.
                    if ((strlen(data_crc_analysis_prefix) != 0) && (attempt == 0)) {
                        char analysis_filename[FILENAME_MAX];
                        snprintf(analysis_filename, FILENAME_MAX, "%s_%d_%d_%d.bin", data_crc_analysis_prefix, j, k, physical_sectors[i]);
                        FILE* analysis_fd = fopen(analysis_filename, "wb");
                        if (analysis_fd != NULL) {
                            fwrite(raw_sectors[i].data_area, sizeof(uint8_t), controller_params->data_area_length, analysis_fd);
                        } else {
                            printf("Could not open %s for writting\n", analysis_filename);
                        }
                    }

                    // Try to process the sector
                    int decode_status = controller_params->process_sector(
                        &drive_params,
                        &raw_sectors[i],
                        &processed_sectors[i]
                    );

                    // Successful read
                    if (!decode_status) {
                        if (!processed_sectors[i].marked_spare && !processed_sectors[i].marked_bad) {

                        // Determine if we have already processed this sector successfully 
                        bool new_lba = true;
                        for (int m = 0; m < num_processed_lbas; m++) {
                            if (processed_lbas[m] == processed_sectors[i].lba) {
                                new_lba = false;
                                break;
                            }
                        }

                        if (new_lba) {
                            processed_lbas[num_processed_lbas++] = processed_sectors[i].lba;
                            fseek(extract_fd, processed_sectors[i].lba * (controller_params->data_area_length - 5), SEEK_SET);
                            fwrite(processed_sectors[i].data, sizeof(uint8_t), processed_sectors[i].length, extract_fd);
                            }

                        } else if (processed_sectors[i].relocated) {

                        }

                    } else {
                        printf("Decode error on [%d,%d,%d] = %d\n", j, k, physical_sectors[i], decode_status);
                    }
                }
                attempt++;
            }

            // Report what we could not read in the alloted attempts
            if ((num_processed_lbas < num_expected_lbas) && !stop) {
                printf("Could not read these LBAs\n");
                for (int m = 0; m < num_expected_lbas; m++) {
                    bool good = false;
                    for (int n = 0; n < num_processed_lbas; n++) {
                        if (expected_lbas[m] == processed_lbas[n])
                            good = true;
                    }
                    if (!good)
                        printf("%d ", expected_lbas[m]);
                }
                printf("\n");
            }

        }
    }
    
    fclose(extract_fd);
    
    for (int i = 0; i < allocated_sectors; i++) {
        free(raw_sectors[i].address_area);
        free(raw_sectors[i].data_area);
        free(processed_sectors[i].data);
    }

    free(raw_sectors);
    free(processed_sectors);

    exit(EXIT_SUCCESS);
}