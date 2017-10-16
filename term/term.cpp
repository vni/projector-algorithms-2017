//g++ lodepng.cpp example_sdl.cpp -lSDL -O3 -o showpng

#include "lodepng.h"

#include <iostream>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <vector>
#include <SDL/SDL.h>
#include <libgen.h>

#define IDX(x, y, w, h) ((y)*(w) + (x))

SDL_Surface* scr;

int *create_energy_map(const unsigned char *rgba, unsigned w, unsigned h, int *max_energy_value_p)
{
    int max_energy_value = 0, energy_sum;
    int *emap = new int[w*h]; // energy map
    char pix1, pix2;

    // calculate energy for bottom line
    for (int x = 0; x < w-1; ++x) {
        energy_sum = 0;
        for (int c = 0; c < 4; ++c) {
            pix1 = rgba[4*IDX(x, h-1, w, h) + c];
            pix2 = rgba[4*IDX(x+1, h-1, w, h) + c];
            energy_sum += std::abs(pix1 - pix2);
        }
        emap[IDX(x, h-1, w, h)] = energy_sum;
        if (max_energy_value < energy_sum) {
            max_energy_value = energy_sum;
            //std::cout << "max_energy_value: " << max_energy_value << "\n";
        }
    }

    // calculate energy for right column
    for (int y = 0; y < h-1; ++y) {
        energy_sum = 0;
        for (int c = 0; c < 4; ++c) {
            pix1 = rgba[4*IDX(w-1, y, w, h) + c];
            pix2 = rgba[4*IDX(w-1, y+1, w, h) + c];
            energy_sum += std::abs(pix1 - pix2);
            //energy_sum += abs(rgba[4*(h-1)*w + 4*x+c] - rgba[4*(h-1)*w + 4*(x+1)+c]);
        }
        emap[IDX(w-1, y, w, h)] = energy_sum;
        if (max_energy_value < energy_sum) {
            max_energy_value = energy_sum;
            //std::cout << "max_energy_value: " << max_energy_value << "\n";
        }
    }

    emap[IDX(w-1, h-1, w, h)] = 0; // bottom right pixel doesn't have energy

    // calculate energy for the rest of pixels
    for (int y = 0; y < h-1; ++y) {
        for (int x = 0; x < w-1; ++x) {
            energy_sum = 0;
            for (int c = 0; c < 4; ++c) { // color component
                pix1 = rgba[4*IDX(x, y, w, h) + c];
                pix2 = rgba[4*IDX(x+1, y, w, h) + c];
                energy_sum += std::abs(pix1 - pix2);
            }
            for (int c = 0; c < 4; ++c) { // color component
                pix1 = rgba[4*IDX(x, y, w, h) + c];
                pix2 = rgba[4*IDX(x, y+1, w, h) + c];
                energy_sum += std::abs(pix1 - pix2);
            }
            energy_sum /= 2;
            emap[IDX(x, y, w, h)] = energy_sum;
            if (max_energy_value < energy_sum) {
                max_energy_value = energy_sum;
                //std::cout << "max_energy_value: " << max_energy_value << "\n";
            }
        }
    }

    std::cout << __FUNCTION__ << ": max_energy_value: " << max_energy_value << "\n";
    *max_energy_value_p = max_energy_value;

    return emap;
}

int min3(int a, int b, int c)
{
    if (a > b)
        a = b;
    if (a > c)
        a = c;
    return a;
}

int min2(int a, int b)
{
    return a < b ? a : b;
}

unsigned char *create_cheapest_path(std::vector<int> &path, int *emap, unsigned char *pic, int w, int h)
{
    path.resize(h);
    int *costs = new int[w*h];

    // top line
    for (int x = 0; x < w; ++x)
        costs[x] = emap[x];

    int min;
    for (int y = 1; y < h; ++y) {
        // x == 0
        min = min2(emap[IDX(0, y-1, w, h)], emap[IDX(1, y-1, w, h)]);
        costs[IDX(0, y, w, h)] = emap[IDX(0, y, w, h)] + min;

        // x == w-1
        min = min2(emap[IDX(w-2, y-1, w, h)], emap[IDX(w-1, y-1, w, h)]);
        costs[IDX(w-1, y, w, h)] = emap[IDX(w-1, y, w, h)] + min;

        // 0 < x && x < w
        for (int x = 1; x < w-1; ++x) {
            min = min3(emap[IDX(x-1, y-1, w, h)], emap[IDX(x, y-1, w, h)], emap[IDX(x+1, y-1, w, h)]);
            costs[IDX(x, y, w, h)] = emap[IDX(x, y, w, h)] + min;
        }
    }

    //int path[h];
    ///////////////////////////////////////////////

    int idx = 0;
    //min = costs[IDX(idx, h-1, w, h)];
    for (int x = 1; x < w; ++x) {
        if (costs[IDX(x, h-1, w, h)] < costs[IDX(idx, h-1, w, h)]) {
            //min = costs[IDX(x, h-1, w, h)];
            idx = x;
        }
    }
    path[h-1] = idx;

    for (int y = h-2; y >= 0; --y) {
        path[y] = path[y+1];

        if (path[y+1] > 0 && costs[IDX(path[y+1]-1, y, w, h)] < costs[IDX(path[y+1], y, w, h)])
            path[y] = path[y+1] - 1;

        if (path[y+1] < w-1 && costs[IDX(path[y+1]+1, y, w, h)] < costs[IDX(path[y+1], y, w, h)])
            path[y] = path[y+1] + 1;
    }
    ///////////////////////////////////////////////

    unsigned char *cheapest_path = new unsigned char[w*h*4];
    std::memcpy(cheapest_path, pic, w*h*4);

    //std::cout << "cheapest path:\n";
    for (int y = 0; y < h; ++y) {
        //std::cout << path[y] << ' ';
        cheapest_path[4*IDX(path[y], y, w, h)+1] = 255;
        //cheapest_path[4*IDX(path[y], y, w, h)+0] = 0;
        //cheapest_path[4*IDX(path[y], y, w, h)+1] = 0;
        //cheapest_path[4*IDX(path[y], y, w, h)+2] = 0;
        //cheapest_path[4*IDX(path[y], y, w, h)+3] = 0;
    }
    //std::cout << '\n';

    return cheapest_path;
}

unsigned char *create_energy_rgba(int *emap, int w, int h, int max_energy_value)
{
    unsigned char *pic = new unsigned char[4*w*h];
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            unsigned char sample = (unsigned char)(255.0 * (float)emap[y*w+x] / (float)max_energy_value);
            /*
               pic[IDX(x, y, w, h)] = sample;
               */
            pic[4*IDX(x, y, w, h)+0] = sample;
            pic[4*IDX(x, y, w, h)+1] = sample;
            pic[4*IDX(x, y, w, h)+2] = sample;
            pic[4*IDX(x, y, w, h)+3] = 255;
        }
    return pic;
}

void remove_path(const std::vector<int> &path, unsigned char img[], int emap[], unsigned char ergba[], int w, int h)
{
    int x;
    for (int y = 0; y < h; ++y) {
        //std::cout << "remove_path: y: " << y << '\n';
        x = path[y];
        //std::cout << "before img memove\n";
        memmove(img+4*IDX(x, y, w, h), img+4*IDX(x+1, y, w, h),     4*((w-x-1)+(h-1-y)*w)*sizeof(unsigned char));
        //std::cout << "before emap memove\n";
        memmove(emap+IDX(x, y, w, h), emap+IDX(x+1, y, w, h),         ((w-x-1)+(h-1-y)*w)*sizeof(int));
        //std::cout << "before ergba memove\n";
        memmove(ergba+4*IDX(x, y, w, h), ergba+4*IDX(x+1, y, w, h), 4*((w-x-1)+(h-1-y)*w)*sizeof(unsigned char));
    }
}

void initSDL(void)
{
    //init SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "error, SDL video init failed" << std::endl;
        //return 0;
        return;
    }
    //SDL_Surface* scr = SDL_SetVideoMode(w / jump, h / jump, 32, SDL_HWSURFACE);
    //scr = SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE);
    scr = SDL_SetVideoMode(1920, 1200, 32, SDL_HWSURFACE);
    if(!scr)
    {
        std::cout << "error, no SDL screen" << std::endl;
        //return 0;
        return;
    }
}

int sdl_event_loop(void)
{
    //pause until you press escape and meanwhile redraw screen
    SDL_Event event;
    int done = 0;
    while(done == 0) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) done = 2;
            else if(SDL_GetKeyState(NULL)[SDLK_ESCAPE]) done = 2;
            else if(event.type == SDL_KEYDOWN) done = 1; //press any other key for next image
        }
        SDL_UpdateRect(scr, 0, 0, 0, 0); //redraw screen
        SDL_Delay(2); //pause 5 ms so it consumes less processing power
    }
    return done;
}

void finiSDL(void)
{
    SDL_Quit();
}

int show(const std::string& caption, const unsigned char* rgba, unsigned w, unsigned h)
{
    //avoid too large window size by downscaling large image
    //unsigned jump = 1;
    //if(w / 1024 >= jump) jump = w / 1024 + 1;
    //if(h / 1024 >= jump) jump = h / 1024 + 1;
    SDL_WM_SetCaption(caption.c_str(), NULL); //set window caption

    //plot the pixels of the PNG file
    for(unsigned y = 0; y < h; ++y) {
        for(unsigned x = 0; x < w; ++x) {
            //get RGBA components
            Uint32 r = rgba[4 * y * w + 4 * x + 0]; //red
            Uint32 g = rgba[4 * y * w + 4 * x + 1]; //green
            Uint32 b = rgba[4 * y * w + 4 * x + 2]; //blue
            Uint32 a = rgba[4 * y * w + 4 * x + 3]; //alpha

            ////make translucency visible by placing checkerboard pattern behind image
            //int checkerColor = 191 + 64 * (((x / 16) % 2) == ((y / 16) % 2));
            //r = (a * r + (255 - a) * checkerColor) / 255;
            //g = (a * g + (255 - a) * checkerColor) / 255;
            //b = (a * b + (255 - a) * checkerColor) / 255;

            //give the color value to the pixel of the screenbuffer
            Uint32* bufp;
            //bufp = (Uint32 *)scr->pixels + (y * scr->pitch / 4) / jump + (x / jump);
            bufp = (Uint32 *)scr->pixels + (y * scr->pitch / 4) + x;
            *bufp = 65536 * r + 256 * g + b;
        }
    }
    int done = sdl_event_loop();
    return done == 2 ? 1 : 0;
}

/*shows image with SDL. Returns 1 if user wants to fully quit, 0 if user wants to see next image.*/
int showfile(const char* filename, int N)
{
    std::cout << "showing " << filename << std::endl;

    std::vector<unsigned char> buffer, image;
    lodepng::load_file(buffer, filename); //load the image file with given filename
    unsigned w, h;
    unsigned error = lodepng::decode(image, w, h, buffer); //decode the png

    //stop if there is an error
    if(error)
    {
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        return 0;
    }

    int max_energy_value;
    int *emap = create_energy_map(&image[0], w, h, &max_energy_value);
    unsigned char *ergba = create_energy_rgba(emap, w, h, max_energy_value);
    std::vector<int> path;
    int ret;
    while (w > 0 && N) {
        ret = show(filename, &image[0], w, h);

        show((std::string(filename) + " energy rgba").c_str(), ergba, w, h);

        unsigned char *cheapest_path = create_cheapest_path(path, emap, ergba, w, h);
        show((std::string(filename) + " cheapest path").c_str(), cheapest_path, w, h);

        unsigned char *img = &image[0];
        for (int y = 0; y < h; ++y) {
            img[4*IDX(path[y], y, w, h)+1] = 255;
        }
        ret = show(filename, &image[0], w, h);

        remove_path(path, &image[0], emap, ergba, w, h);
        --w;
        --N;

        delete[] cheapest_path;
    }

    delete[] emap;
    delete[] ergba;

    return ret;
}

int main(int argc, char* argv[])
{
    if(argc <= 2) { // std::cout << "Please enter PNG file name(s) to display" << std::endl;
        std::cerr << "Usage: " << basename(argv[0]) << " M png1 ... pngN\n";
        std::cerr << "  where M       - how many pixels to remove from the picture\n";
        std::cerr << "  png1 ... pngN - list of pictures to process\n";
    }

    int N = atoi(argv[1]);

    initSDL();

    for(int i = 2; i < argc; i++)
    {
        if(showfile(argv[i], N)) return 0;
    }

    finiSDL();
}
