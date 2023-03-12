RGB_MATRIX_EFFECT(INKY_PRIDEFUL)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static HSV prideful_colors[] = {{40, 240, 114}, {0, 0, 155}, {0, 0, 155}, {193, 255, 114}, {193, 255, 114}, {193, 0, 0}, {193, 0, 0}, {40, 240, 114}};
static int color_count = sizeof(prideful_colors) / sizeof(prideful_colors[0]);
static uint8_t led_max_x = 224;

static float EASE_IN_EXPO(float x) {
    return x == 0 ? 0 : pow(2, 10 * x - 10);
}

// hugs and kisses to SirTimmyTimbit on github: https://github.com/SirTimmyTimbit/customizable-gradient-effect-for-drop-alt
static HSV STT_INTERPOLATE_HSV(float step, HSV gradient_0, HSV gradient_100) {
    uint8_t cw, ccw;
    HSV color;
    float eased_step = EASE_IN_EXPO(step);

    if (gradient_0.v == 0) {
        color.h = gradient_100.h;
        color.s = gradient_100.s;
    } else if (gradient_100.v == 0) {
        color.h = gradient_0.h;
        color.s = gradient_0.s;
    } else {
        cw = (gradient_0.h >= gradient_100.h) ? 255 + gradient_100.h - gradient_0.h : gradient_100.h - gradient_0.h;  // Hue range is 0 to 255.
        ccw = (gradient_0.h >= gradient_100.h) ? gradient_0.h - gradient_100.h : 255 + gradient_0.h - gradient_100.h;

        if (cw < ccw) { // going clockwise
            color.h = gradient_0.h + (uint8_t)(eased_step * cw);
        } else { // Going counter clockwise
            color.h = gradient_0.h - (uint8_t)(eased_step * ccw);
        }

        color.s = gradient_0.s + eased_step * (gradient_100.s - gradient_0.s);
    }

    color.v = round(gradient_0.v + eased_step * (gradient_100.v - gradient_0.v));
    if (color.v != 0) {
        // colors tend to get inaccurate if set too dark, yellow may turn red for instance
        color.v = color.v >= 10 ? color.v : 0;
    }

    return color;
}

static HSV INKY_PRIDEFUL_math(uint8_t led_x, uint8_t min_x, uint8_t max_x) {
    if (led_x < min_x) {
        return prideful_colors[0];
    } else if (led_x > max_x) {
        return prideful_colors[color_count - 1];
    } else {
        float step = ((float) led_x - min_x) / (max_x - min_x);
        float color_step = (color_count - 1) * step;
        HSV color_1 = prideful_colors[(int) floor(color_step)];
        HSV color_2 = prideful_colors[(int) floor(color_step + 1)];

        return STT_INTERPOLATE_HSV(fmod(color_step, 1), color_1, color_2);
    }
}

static bool INKY_PRIDEFUL(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        uint8_t led_x = g_led_config.point[i].x;

        if (rgb_matrix_config.speed != 0) {
            uint8_t time = scale16by8(g_rgb_timer, qadd8(rgb_matrix_config.speed / 4, 1));
            led_x -= time;
        } else if (led_x > led_max_x) {
            led_x -= led_max_x;
        }

        HSV hsv = INKY_PRIDEFUL_math(led_x, 0, 224);
        RGB rgb = rgb_matrix_hsv_to_rgb(hsv);

        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }

    return rgb_matrix_check_finished_leds(led_max);
}
#endif
