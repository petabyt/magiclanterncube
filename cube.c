#include <stdio.h>
#include <dryos.h>
#include <module.h>
#include <menu.h>
#include <config.h>
#include <bmp.h>
#include <console.h>
#include <math.h>

#define FOV 90
#define OFFSET 150

int running = 1;

float point[3] = {0, 0, 0};
float xy[2] = {0};

float angles[3] = {1, 1, 1};
float distance = 5;

float toRad(float angle)
{
    return angle * (M_PI / 180);
}

void drawLine(float x0, float y0, float x1, float y1)
{
    draw_line(x0, y0, x1, y1, COLOR_WHITE);
}

void rotate(float x, float y, float angle)
{
    xy[0] = (x * cosf(angle)) + (y * sinf(angle));
    xy[1] = (y * cosf(angle)) - (x * sinf(angle));
}

void rotatePoint(float x, float y, float z)
{
    rotate(x, y, angles[2]);
    point[0] = xy[0];
    point[1] = xy[1];
    rotate(point[0], z, angles[1]);
    point[0] = xy[0];
    point[2] = xy[1];
    rotate(point[2], point[1], angles[0]);
    point[2] = xy[0];
    point[1] = xy[1];
}

void renderLine(float x1, float y1, float z1, float x2, float y2, float z2)
{
    float multiplier;

    float line[4] = {0};

    rotatePoint(x1, y1, z1);
    multiplier = 240 / ((point[2] + distance) * tanf(toRad(FOV) / 2));
    line[0] = point[0] * multiplier;
    line[1] = point[1] * multiplier;

    rotatePoint(x2, y2, z2);
    multiplier = 240 / ((point[2] + distance) * tanf(toRad(FOV) / 2));
    line[2] = point[0] * multiplier;
    line[3] = point[1] * multiplier;

    drawLine(
        line[0] + OFFSET,
        line[1] + OFFSET,
        line[2] + OFFSET,
        line[3] + OFFSET
    );
}

static void cube_task()
{
    if (!running)
    {
        return;
    }

    TASK_LOOP
    {
        if (!running)
        {
            return;
        }

        msleep(30);
        bmp_fill(COLOR_BLACK, 0, 0, 350, 350);
        bmp_printf(FONT_LARGE, 10, 10, "Cool 3D stuffs!!");
        renderLine(-1, -1, -1, 1, -1, -1);
        renderLine(-1, 1, -1, 1, 1, -1);
        renderLine(-1, -1, -1, -1, 1, -1);
        renderLine(1, -1, -1, 1, 1, -1);
        renderLine(-1, 1, 1, 1, 1, 1);
        renderLine(-1, -1, 1, 1, -1, 1);
        renderLine(-1, -1, 1, -1, 1, 1);
        renderLine(1, -1, 1, 1, 1, 1);
        renderLine(-1, -1, -1, -1, -1, 1);
        renderLine(1, -1, -1, 1, -1, 1);
        renderLine(1, -1, -1, 1, -1, 1);
        renderLine(1, 1, -1, 1, 1, 1);
        renderLine(-1, 1, -1, -1, 1, 1);

        /* X in middle */
        renderLine(-1, -1, -1, 1, 1, 1);
        renderLine(1, -1, 1, -1, 1, -1);

        angles[0] += 0.02;
        angles[1] += 0.02;
    }
}

static unsigned int cube_keypress(unsigned int key)
{
    if (key == MODULE_KEY_Q)
    {
        running = 0;
    }

    return 1;
}

static struct menu_entry cube_menu[] =
{
    {
        .name       = "Wireframe Cube",
        .select     = run_in_separate_task,
        .priv       = cube_task,
        .help       = "Simple Wireframe Demo",
    },
};

static unsigned int cube_init()
{
    menu_add("Games", cube_menu, COUNT(cube_menu));
    return 0;
}

static unsigned int cube_deinit()
{
    return 0;
}

MODULE_CBRS_START()
MODULE_CBR(CBR_KEYPRESS, cube_keypress, 0)
MODULE_CBRS_END()

MODULE_INFO_START()
MODULE_INIT(cube_init)
MODULE_DEINIT(cube_deinit)
MODULE_INFO_END()
