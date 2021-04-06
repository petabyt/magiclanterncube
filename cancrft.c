#include <stdio.h>
#include <dryos.h>
#include <module.h>
#include <menu.h>
#include <config.h>
#include <bmp.h>
#include <console.h>
#include <math.h>

#define FOV 90
#define OFFSET 100

float point[3] = {0, 0, 0};
float xy[2] = {0};

float angles[3] = {1, 1, 1};
float distance = 5;

float toRad(float angle) {
	return angle * (M_PI / 180);
}

void drawLine(float x0, float y0, float x1, float y1) {
	draw_line(x0, y0, x1, y1, 1);
}

void rotate(float x, float y, float angle) {
	xy[0] = (x * cosf(angle)) + (y * sinf(angle));
	xy[1] = (y * cosf(angle)) - (x * sinf(angle));
}

void rotatePoint(float x, float y, float z) {
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

void renderLine(float x1, float y1, float z1, float x2, float y2, float z2) {
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

static void cos_task()
{
	msleep(1000);
	TASK_LOOP {
		clrscr();
		renderLine(-1, -1, -1, 1, -1, -1);
		renderLine(-1, 1, -1, 1, 1, -1);
		renderLine(-1, -1, -1, -1, 1, -1);
		renderLine(1, -1, -1, 1, 1, -1);
	
		// Draw Inner box
		renderLine(-1, 1, 1, 1, 1, 1);
		renderLine(-1, -1, 1, 1, -1, 1);
		renderLine(-1, -1, 1, -1, 1, 1);
		renderLine(1, -1, 1, 1, 1, 1);
	
		// Draw
		renderLine(-1, -1, -1, -1, -1, 1);
		renderLine(1, -1, -1, 1, -1, 1);
		renderLine(1, -1, -1, 1, -1, 1);
		renderLine(1, 1, -1, 1, 1, 1);
		renderLine(-1, 1, -1, -1, 1, 1);
		angles[0] += 0.1;
		angles[1] += 0.1;
		msleep(30);
	}
}

static struct menu_entry cos_menu[] =
{
    {
        .name       = "Wireframe Cube",
        .select     = run_in_separate_task,
        .priv       = cos_task,
        .help       = "Stupid 3D thing",
    },
};

/* This function is called when the module loads. */
/* All the module init functions are called sequentially,
 * in alphabetical order. */
static unsigned int cos_init()
{
    menu_add("Debug", cos_menu, COUNT(cos_menu));
    return 0;
}

/* Note: module unloading is not yet supported;
 * this function is provided for future use.
 */
static unsigned int cos_deinit()
{
    return 0;
}

MODULE_INFO_START()
    MODULE_INIT(cos_init)
    MODULE_DEINIT(cos_deinit)
MODULE_INFO_END()
