/*
 * Copyright (C) 2016 Unwired Devices
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Default application that shows a functionality of LoRa-Star gateway
 *
 * @author      Eugene Ponomarev
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "shell_commands.h"
#include "thread.h"
#include "xtimer.h"
#include "lpm.h"
#include "periph/rtc.h"
#include "random.h"

#include "l1-nvram-eeprom.h"

#include "board.h"

#include "ls-mac-types.h"
#include "sx1276.h"

#include "main.h"
#include "config.h"

static nvram_t nvram;

void print_logo(void)
{
    puts("                                                .@                           @  ");
    puts("                                                                             @  ");
    puts("  @@@           %@@,     &@**%@. .#    ./   .#  .@   #@*.   *@@@@@,    @#%.%%@  ");
    puts("  @@@           %@@,    @#    .&  .# ..@.  .#*  .@  /#    .@.    ,%   @.    .@  ");
    puts("  @@@           %@@,    @*    .@  .&,,&  @.#%   .@  %*    .@&&&&&&&*  @      @  ");
    puts("  @@@           %@@,    @*    .@   .@@   .@%    .@  %*    ,@          *@,   ,@, ");
    puts("  @@@           %@@,    *.     *                .#  ,.      %@&&@#     **,*.@   ");
    puts("  @@@           %@@,															  ");
    puts("  @@@   .,,,,,,,...            %@@@%   %     .# *%   .#@@@@,    *@@@@,    @@@*  ");
    puts("  @@@   @@@@@@@@@@@@@@&.     %&     &%  @    @  .@  &*        .@.    ,@  @      ");
    puts("  @@@   @@@     /.. *@@@@.   @&&&&&&&&  ,&  @.  .@  @         %@&&&&&&&*  #@(   ");
    puts("  &@@*  @@@     @@@   (@@@   @#          (@@/   .@  @.        ,@             @  ");
    puts("   @@@. @@@    @@@#    #@@%   .@@&@@.     /*    .@   /@@&@@(    %@&&@#   &@&@*  ");
    puts("    @@% @@@ @@@@@.     .@@@                                                     ");
    puts("        @@@ ####/#####/ @@@ ##################################################  ");
    puts("        @@@            *@@&                                                     ");
    puts("        @@@            @@@,                                                     ");
    puts("        @@@          *@@@#                                                      ");
    puts("        @@@,...,,#&@@@@@                                                        ");
    puts("        @@@@@@@@@@@%,                                                           ");
    puts("                                                                                ");
    printf("Version: %s\n", FIRMWARE_VERSION);
    puts("");
}

void blink_led(void)
{
    volatile int i;

    LED0_OFF;

    for (i = 0; i < 5; i++) {
        LED0_TOGGLE;
        xtimer_usleep(50000);

        LED0_TOGGLE;
        xtimer_usleep(50000);
    }

    LED0_OFF;
}

static shell_command_t shell_commands[10] = {};

static void init_role(config_role_t role) {
	switch (role) {
	case ROLE_NODE:
		init_node((shell_command_t **) &shell_commands);
		break;

	case ROLE_NO_EUI64:
		init_no_eui64((shell_command_t **) &shell_commands);
		break;

	default:
	case ROLE_NO_CFG:
		init_no_cfg((shell_command_t **) &shell_commands);

		break;
	}
}

/*
static bool check_button(void) {
    gpio_init(UNWD_CONNECT_BTN, GPIO_IN);

    xtimer_usleep(1e3 * 1000);

    if (!gpio_read(UNWD_CONNECT_BTN)) {
    	int i;
    	for (i = 0; i < 10; i++) {
    		if (gpio_read(UNWD_CONNECT_BTN))
    			return false;

    		xtimer_usleep(1e3 * 1000);
    	}

    	return true;
    }

    return false;
}*/

int main(void)
{
    print_logo();
    xtimer_init();

    nvram_l1_eeprom_init(&nvram);

    /*if (check_button()) {
    	puts("[!] Button press detected, resetting config...");

    	blink_led();
    	blink_led();
    	blink_led();

    	config_reset_nvram(&nvram);
    }*/

    /* Check EUI64 */
    if (!load_eui64_nvram(&nvram)) {
    	puts("[config] No EUI64 defined for this device. Please provide EUI64 and reboot to apply changes.");
    }

    /* It's first launch or config memory is corrupted */
    if (!load_config_nvram(&nvram)) {
    	puts("[config] No valid configuration found in NVRAM. It's either first launch or NVRAM content is corrupted.");
    	puts("[config] Please provide APPID64 and JOINKEY for this device.");

    	config_reset_nvram(&nvram);
    } else {
    	puts("[config] Configuration loaded from NVRAM");
    }

    init_role(config_get_role());

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}