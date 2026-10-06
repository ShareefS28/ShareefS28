/**
 * gcc spinning_donut.c -o donut -lm 
 * or 
 * gcc spinning_donut.c -o donut.exe -lm
 * then
 * ffmpeg -y -framerate 30 -i "asset\png\frame_%03d.png" -vf "palettegen=stats_mode=diff" palette.png !NOT WORK!
 * then
 * ffmpeg -y -framerate 30 -i "asset\png\frame_%03d.png" -i palette.png -lavfi "paletteuse=dither=sierra2_4a" -loop 0 donut.gif !NOT WORK!
 * 
 * conda env export --no-builds --name <env_name> | findstr /V "^prefix: " > <file_name>.yml
 * conda env create --file <conda_env_file>.yml or conda env create --file environment.yml --name <env_name>
 */

#include <stdio.h>
#include <math.h>
#include <string.h>

#define WIDTH 80
#define HEIGHT 24
#define FRAMES 300

const char *shades = ".,-~:;=!*#$@";

int main(void)
{
    double A = 0.0;
    double B = 0.0;

    for (int frame = 0; frame < FRAMES; frame++)
    {
        double zbuffer[WIDTH * HEIGHT];
        char output[WIDTH * HEIGHT];

        for (int i = 0; i < WIDTH * HEIGHT; i++)
        {
            zbuffer[i] = 0.0;
            output[i] = ' ';
        }

        for (int theta_i = 0; theta_i < 628; theta_i += 3) // Denser sampling
        {
            double theta = theta_i / 100.0;

            double sin_theta = sin(theta);
            double cos_theta = cos(theta);

            for (int phi_i = 0; phi_i < 628; phi_i += 1) // Denser sampling
            {
                double phi = phi_i / 100.0;

                double sin_phi = sin(phi);
                double cos_phi = cos(phi);

                /*
                 * Torus
                 */
                double circle_x = cos_theta + 2.0;
                double circle_y = sin_theta;

                /*
                 * 3D rotation
                 */
                double x =
                    circle_x *
                        (cos_phi * cos(B)
                         + sin_phi * sin(A) * sin(B))
                    - circle_y * cos(A) * sin(B);

                double y =
                    circle_x *
                        (cos_phi * sin(B)
                         - sin_phi * sin(A) * cos(B))
                    + circle_y * cos(A) * cos(B);

                double z =
                    circle_x * sin_phi * cos(A)
                    + circle_y * sin(A);

                z += 5.0;

                if (z <= 0.0)
                    continue;

                double ooz = 1.0 / z;

                /*
                 * Projection
                 */
                int xp = (int)(WIDTH / 2 + 30 * ooz * x);
                int yp = (int)(HEIGHT / 2 - 15 * ooz * y);

                if (xp < 0 || xp >= WIDTH ||
                    yp < 0 || yp >= HEIGHT)
                    continue;

                int index = xp + yp * WIDTH;

                /*
                 * Z-buffer
                 */
                if (ooz > zbuffer[index])
                {
                    zbuffer[index] = ooz;

                    /*
                     * Lighting
                     */
                    double brightness =
                        cos_phi * cos_theta * sin(B)
                        - cos_phi * sin_theta * sin(A)
                        - sin_phi * cos_theta * cos(B)
                        + sin_phi * sin_theta * sin(A) * cos(B);

                    int shade = (int)((brightness + 1.0) * 5.5);

                    if (shade < 0) shade = 0;

                    if (shade > 11) shade = 11;

                    output[index] = shades[shade];
                }
            }
        }

        /*
         * Save frame
         */
        char filename[64];

        sprintf(filename, "asset/frames/frame_%03d.txt", frame);

        FILE *file = fopen(filename, "w");

        if (!file)
        {
            perror("Could not create frame");
            return 1;
        }

        for (int y = 0; y < HEIGHT; y++)
        {
            fwrite(
                &output[y * WIDTH],
                1,
                WIDTH,
                file
            );

            fputc('\n', file);
        }

        fclose(file);

        /*
         * Rotate
         */
        A += 2.0 * 3.14159265358979 * 2.0 / FRAMES;  // 2 full turns of A
        B += 2.0 * 3.14159265358979 * 1.0 / FRAMES;  // 1 full turn of B
    }

    printf("Generated %d frames.\n", FRAMES);

    return 0;
}