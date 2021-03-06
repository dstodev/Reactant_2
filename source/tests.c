#include "tests.h"


void print_result(const char * label, unsigned char rval, int width) {
    const char *success = "GOOD";
    const char *failure = "FAIL";
    int pad_size = (sizeof(char) * (width - strlen(label) - strlen(success) - 6));
    char *pad = malloc(pad_size);
    memset(pad, (int) '.', pad_size - 1);
    pad[pad_size - 1] = 0;
    if (rval) {
        debug_output("%s %s [ " RED "%s" DEFAULT " ]\n", label, pad, failure);
    } else {
        debug_output("%s %s [ " GREEN "%s" DEFAULT " ]\n", label, pad, success);
    }
    free(pad);
}

int test_all(WINDOW *window) {
    debug_control(ENABLE);
    endwin();
    system("clear");

    print_result("SPI", test_spi(), getmaxx(window));
    print_result("I2C", test_i2c(), getmaxx(window));
    print_result("Humidity", test_humidity(), getmaxx(window));
    print_result("Light", test_light(), getmaxx(window));
    print_result("Pressure", test_pressure(), getmaxx(window));
    print_result("Temperature", test_temperature(), getmaxx(window));
    print_result("AES256", test_aes(), getmaxx(window));
    print_result("SHA256", test_sha(), getmaxx(window));
    print_result("Message", test_message(), getmaxx(window));
    print_result("Channels", test_channels(), getmaxx(window));


    debug_output("Press ENTER to continue!");
    while((getchar() != '\n'));
    return 0;
}

int test_spi_cb(WINDOW *window) {
    debug_control(ENABLE);
    endwin();
    system("clear");
    print_result("SPI", test_spi(), getmaxx(window));
    debug_output("Press ENTER to continue!");
    while ((getchar() != '\n'));
    return 0;
}

int test_spi() {
    int rval = 0;
    debug_control(DISABLE);

    rval |= peripheral_spi_init();
    rval |= peripheral_spi_term();

    debug_control(ENABLE);
    return rval;
}

int test_i2c_cb(WINDOW *window) {
    debug_control(ENABLE);
    endwin();
    system("clear");
    print_result("I2C", test_i2c(), getmaxx(window));
    debug_output("Press ENTER to continue!");
    while ((getchar() != '\n'));
    return 0;
}

int test_i2c() {
    int rval = 0;
    debug_control(DISABLE);

    rval |= peripheral_i2c_init();
    rval |= peripheral_i2c_term();

    debug_control(ENABLE);
    return rval;
}

int test_humidity_cb(WINDOW *window) {
    debug_control(ENABLE);
    endwin();
    system("clear");
    print_result("Humidity", test_humidity(), getmaxx(window));
    debug_output("Press ENTER to continue!");
    while ((getchar() != '\n'));
    return 0;
}

int test_humidity() {
    int rval = 0;
    debug_control(DISABLE);

    rval |= peripheral_spi_init();

    // Humidity ADC value should not be less than 50
    rval |= (mcp3008_read_channel(2) < 50);
    debug_output("%d\n", mcp3008_read_channel(2));

    rval |= peripheral_spi_term();

    debug_control(ENABLE);
    return rval;
}

int test_light_cb(WINDOW *window) {
    debug_control(ENABLE);
    endwin();
    system("clear");
    print_result("Light", test_light(), getmaxx(window));
    debug_output("Press ENTER to continue!");
    while ((getchar() != '\n'));
    return 0;
}

int test_light() {
    int rval = 0;
    int rval2 = 0;
    debug_control(DISABLE);

    rval |= peripheral_i2c_init();

    // Ensure valid light sensor ID register
    if ((rval2 = smbus_read_byte(TSL2561_ID)) != 0x50) {
        rval |= 1;
    }

    /*// Ensure light can be enabled and disabled
    if ((rval2 = tsl2561_enable()) == -1) {
        // "Already enabled" should not be treated as fail
        rval2 = 0;
    }
    rval |= rval2;
    if ((rval2 = tsl2561_disable()) == -1) {
        // "Already disabled" should not be treated as fail
        rval2 = 0;
    }
    rval |= rval2;
    */
    rval |= peripheral_i2c_term();

    debug_control(ENABLE);
    return rval;
}

int test_pressure_cb(WINDOW *window) {
    debug_control(ENABLE);
    endwin();
    system("clear");
    print_result("Pressure", test_pressure(), getmaxx(window));
    debug_output("Press ENTER to continue!");
    while ((getchar() != '\n'));
    return 0;
}

int test_pressure() {
    int rval = 0;
    debug_control(DISABLE);

    // Strictly uses SPI communications with analog voltage output
    rval |= peripheral_spi_init();

    // Pressure ADC value should not be less than 50
    rval |= (mcp3008_read_channel(0) < 50);
    debug_output("%d\n", mcp3008_read_channel(0));

    rval |= peripheral_spi_term();

    debug_control(ENABLE);
    return rval;
}

int test_temperature_cb(WINDOW *window) {
    debug_control(ENABLE);
    endwin();
    system("clear");
    print_result("Temperature", test_temperature(), getmaxx(window));
    debug_output("Press ENTER to continue!");
    while ((getchar() != '\n'));
    return 0;
}

int test_temperature() {
    int rval = 0;
    debug_control(DISABLE);

    rval |= peripheral_spi_init();

    // Temperature ADC value should not be less than 50
    rval |= (mcp3008_read_channel(1) < 50);
    debug_output("%d\n", mcp3008_read_channel(1));

    rval |= peripheral_spi_term();

    debug_control(ENABLE);
    return rval;
}

int test_aes_cb(WINDOW *window) {
    debug_control(ENABLE);
    endwin();
    system("clear");
    print_result("AES256", test_aes(), getmaxx(window));
    debug_output("Press ENTER to continue!");
    while ((getchar() != '\n'));
    return 0;
}

int test_aes() {
    int rval = 0;
    struct AES_ctx context;
    char buffer[64];
    char * str = "This is a test!";
    char * key = "12345678901234567890123456789012";
    char * iv = "1234567890123456";
    debug_control(DISABLE);

    strcpy(buffer, str);

    // Encrypt message
    AES_init_ctx_iv(&context, (const uint8_t *) key, (const uint8_t *) iv);
    AES_CBC_encrypt_buffer(&context, (uint8_t *) buffer, sizeof(buffer));

    if (strcmp(buffer, str) == 0) {
        rval = 1;
    }

    // Decrypt message
    AES_init_ctx_iv(&context, (const uint8_t *) key, (const uint8_t *) iv);
    AES_CBC_decrypt_buffer(&context, (uint8_t *) buffer, sizeof(buffer));

    if (strcmp(buffer, str) != 0) {
        rval = 1;
    }

    debug_control(ENABLE);
    return rval;
}

int test_sha_cb(WINDOW *window) {
    debug_control(ENABLE);
    endwin();
    system("clear");
    print_result("SHA256", test_sha(), getmaxx(window));
    debug_output("Press ENTER to continue!");
    while ((getchar() != '\n'));
    return 0;
}

int test_sha() {
    int rval = 0;
    char * str = "This is a test!";
    char * hash = 0;
    debug_control(DISABLE);

    hash = (char *) message_hash(str);
    if (hash == NULL || strcmp(hash, str) == 0) {
        rval = 1;
    }
    if (hash) {
        free(hash);
    }

    debug_control(ENABLE);
    return rval;
}

int test_message_cb(WINDOW *window) {
    debug_control(ENABLE);
    endwin();
    system("clear");
    print_result("Message", test_message(), getmaxx(window));
    debug_output("Press ENTER to continue!");
    while ((getchar() != '\n'));
    return 0;
}

int test_message() {
    int rval = 0;
    message_t message;
    char * str = "This is a test!";
    char * key = "12345678901234567890123456789012";
    char * iv = "1234567890123456";
    debug_control(DISABLE);

    rval |= message_initialize(&message);

    strcpy(message.payload, str);

    rval |= message_pack(&message, key, iv);
    message_debug_hex(message.message_string);
    rval |= message_unpack(&message, key, iv);
    message_debug_hex(message.message_string);

    if (strcmp(message.payload, str) != 0) {
        rval = 1;
    }

    debug_control(ENABLE);
    return rval;
}

int test_channels_cb(WINDOW *window) {
    debug_control(ENABLE);
    endwin();
    system("clear");
    print_result("Channels", test_channels(), getmaxx(window));
    debug_output("Press ENTER to continue!");
    while ((getchar() != '\n'));
    return 0;
}

static int _returns;
static void _test_channels_callback(char *message) {
    _returns += 1;
}

typedef struct _gencfg_t {
        char ip[16];
        short port;
        char key[33];
        char iv[17];
} gencfg_t;

static int _gencfg_handler(const mTCHAR *section, const mTCHAR *key, const mTCHAR *value, void *user) {
    gencfg_t *gencfg = (gencfg_t *)user;

    #define MATCH(s, k) (strcmp(s, section) == 0 && strcmp(k, key) == 0)
    if (MATCH("general", "core-ip")) {
        strcpy(gencfg->ip, value);
    } else if (MATCH("general", "port")) {
        gencfg->port = (short) atoi(value);
    } else if (MATCH("security", "key")) {
        strcpy(gencfg->key, value);
    } else if (MATCH("security", "iv")) {
        strcpy(gencfg->iv, value);
    }
    return 1;
}

int test_channels() {
    int rval = 0;
    core_t core;

    struct timespec delay;
    delay.tv_sec = 1;
    delay.tv_nsec = 0;

    gencfg_t config;
    if (ini_browse(&_gencfg_handler, &config, CONF_INI) < 0) {
        debug_output("Failed to load configuration settings!\n");
        rval |= 1;
    }

    _returns = 0;
    debug_control(DISABLE);

    if (!start_node_client(&core, 0x941, config.ip, config.port, config.key, config.iv)) {
        subscribe(&core, "Test-1", _test_channels_callback);
        subscribe(&core, "Test-2", _test_channels_callback);
        subscribe(&core, "Test-3", _test_channels_callback);
        subscribe(&core, "Test-4", _test_channels_callback);

        publish(&core, "Test-1", "Channel test publish!");
        publish(&core, "Test-2", "Channel test publish!");
        publish(&core, "Test-3", "Channel test publish!");
        publish(&core, "Test-4", "Channel test publish!");

        // Wait one second
        nanosleep(&delay, NULL);

        rval |= (_returns != 4);
        debug_output("%d\n", _returns);

        stop_node_client(&core);

    } else {
        rval |= 1;
    }
    debug_control(ENABLE);
    return rval;
}

/* ################################################################################################################## */
/* ################################################################################################################## */

void spi_test() {
#ifdef __arm__
    if (!peripheral_init()) {
        int rval = 0;
        float temperature = 0;

        while (rval < 850) {
            temperature = mcp3008_read_channel(1);
            fprintf(stderr, "Ch %d: %d \tCh %d: %d\n", 1, rval = mcp3008_read_channel(0), 2, (int) temperature);

            temperature = (225.0 * temperature) / 256.0 - 58.0;
            fprintf(stderr, "Temperature: %f\n\n", temperature);

            bcm2835_delay(100);
        }
        peripheral_term();
    } else {
        fprintf(stderr, "%s\n", "spi_test() failed! Could not initialize peripherals!");
    }
#endif
}

void i2c_test() {
#ifdef __arm__
    char rval;
    short ch0, ch1;

    // Initialize peripherals
    if (!peripheral_init()) {
        // Setup timing register (402ms integration time, 16x gain)
        smbus_write_byte(TSL2561_TIMING, TSL2561_INTEGRATION_402 | TSL2561_GAIN_16);

        // Ensure valid ID register
        if ((rval = smbus_read_byte(TSL2561_ID)) != 0x50) {
            fprintf(stderr, "%s: 0x%x\n", "Returned invalid ID register read!", rval);
        }

        // Enable light sensor
        tsl2561_enable();

        do {
            // Wait 403 ms (because of the 402ms integration time)
            usleep(0.403 * S_MUL);

            // Read channel 0 (broadband)
            ch0 = smbus_read_word(TSL2561_WORD | TSL2561_DATA0LOW);

            // Read channel 1 (IR)
            ch1 = smbus_read_word(TSL2561_WORD | TSL2561_DATA1LOW);

            fprintf(stderr, "Ch0 (broadband): %d \tCh1 (IR): %d\n", ch0, ch1);

        } while (ch0 > 2000);

        // Disable light sensor
        tsl2561_disable();

        // Terminate peripherals
        peripheral_term();
    } else {
        fprintf(stderr, "%s\n", "i2c_test() failed! Could not initialize peripherals!");
    }
#endif
}

int _test_callback(WINDOW * window) {
    int y, x;
    getmaxyx(stdscr, y, x);
    mvwprintw(window, y / 2, x / 2 - 7, "This is a test!");

    return 0;
}

int _configure_callback_test(WINDOW * PH(window)) {
    int rval = 0;

    panel_t * panel = create_panel("Operation", 2, 3);
    add_panel_button(panel, create_button("Back", NULL));

    menu_t * menu = create_menu("Reactant Configuration");
    add_menu_panel(menu, panel);

    rval = operate_menu(menu);

    free_menu(menu);
    if (rval == UI_QUIT) {
        return UI_QUIT;
    }
    return rval;
}

int _prompt_callback_test(WINDOW * window) {
    char test[5] = { 0 };
    prompt(window, "Enter a new value:", test, sizeof(test));
    mvwprintw(window, 6, 15, " %s ", test);
    return 0;
}

void ui_test() {
    // ################
    // # Run Reactant #
    // ################
    curses_init();

    // Menu items
    panel_t * panels[4];

    panels[0] = create_panel("Operation", 2, 3); // 2, 3
    add_panel_button(panels[0], create_button("Start Reactant Core server", _test_callback));
    add_panel_button(panels[0], create_button("Configure", _configure_callback_test));
    add_panel_button(panels[0], create_button("Exit", NULL));

    panels[1] = create_panel("Other", 2, 40);
    add_panel_button(panels[1], create_button("About", _test_callback));
    add_panel_button(panels[1], create_button("Test Reactant Node device", _test_callback));

    panels[2] = create_panel("TESTPANEL1", 7, 3);
    add_panel_button(panels[2], create_button("Test Prompt", _prompt_callback_test));

    panels[3] = create_panel("TESTPANEL2", 7, 40);
    add_panel_button(panels[3], create_button("TESTBUTTON4", _test_callback));
    add_panel_button(panels[3], create_button("TESTBUTTON5", _test_callback));
    add_panel_button(panels[3], create_button("TESTBUTTON6", _test_callback));

    panels[0]->selected = 1;
    panels[0]->items[0]->selected = 1;

    menu_t * menu = create_menu("Reactant Primary Control");
    add_menu_panel(menu, panels[0]);
    add_menu_panel(menu, panels[1]);
    add_menu_panel(menu, panels[2]);
    add_menu_panel(menu, panels[3]);

    operate_menu(menu);

    free_menu(menu);
    curses_term();
}

void queue_test() {
    queue_t q;

    int * items[10];
    int * rval = 0;

    debug_control(ENABLE);

    queue_construct(&q, 10);

    for(int i = 0; i < q.capacity; ++i) {
        items[i] = malloc(sizeof(int));
        *items[i] = i;
        queue_check(enqueue(&q, items[i]));
    }
    printf("Capacity: %d, Size: %d\n", (int) q.capacity, (int) q.size);

    printf("test\n");
    queue_check(enqueue(&q, items[0]));

    for(int i = 0; i < 5; ++i) {
        queue_check(dequeue(&q, (void **) &rval));
        printf("%d\n", *rval);
    }

    for(int i = 0; i < 5; ++i) {
        queue_check(enqueue_blocking(&q, items[7]));
    }

    for(int i = 0; i < q.capacity; ++i) {
        queue_check(dequeue_blocking(&q, (void **) &rval));
        printf("%d\n", *rval);
    }

    queue_check(dequeue(&q, (void **) &rval));
    printf("%d\n", *rval);

    for (int i = 0; i < q.capacity; ++i) {
        free(items[i]);
    }
    queue_destruct(&q);
}

void message_test() {
    message_t message1, message2;
    message_initialize(&message1);
    message_initialize(&message2);

    message1.bytes_remaining = 0x1234;
    message1.source_id = 0x567890AB;
    strcpy(message1.payload, "Hello!");

    message_pack(&message1, NULL, NULL);
    strcpy(message2.message_string, message1.message_string);
    message_unpack(&message2, NULL, NULL);

    fprintf(stderr, "%x %x %s\n", message2.bytes_remaining, message2.source_id, message2.payload);
}
