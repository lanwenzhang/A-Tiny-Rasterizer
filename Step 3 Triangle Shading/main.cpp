#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
Model* model = NULL;
const int width = 800;
const int height = 800;

// Shade a triangle
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color)
{
    // Check whether the size of triangle is zero
    if (t0.y == t1.y && t0.y == t2.y) return;

    // Order the vertex based on the y-value
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);

    int total_height = t2.y - t0.y;

    for (int i = 0; i < total_height; i++)
    {
        // split the triangle into two triangles by the value of t1.y
        bool second_half = i > t1.y - t0.y || t1.y == t0.y;
        int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
        float alpha = (float)i / total_height;
        float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height;

        // Calculate the A, B coordinates
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;

        // If A is on the right of B, switch A and B to make sure shading from left to right
        if (A.x > B.x) std::swap(A, B);
        // Shade the current height
        for (int j = A.x; j <= B.x; j++)
        {
            image.set(j, t0.y + i, color);
        }
    }
}



int main(int argc, char** argv) {
    // Load the model
    if (2 == argc) {
        model = new Model(argv[1]);
    }
    else {
        model = new Model("D:/Career/Knowledge/Graphics_Programming/TinyRasterizer/obj/african_head.obj");
    }

    // Construct the tgaimage
    TGAImage image(width, height, TGAImage::RGB);

    // Define the direction of light
    Vec3f light_dir(0, 0, -1);
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        Vec2i screen_coords[3];
        Vec3f world_coords[3];
        for (int j = 0; j < 3; j++)
        {
            Vec3f v = model->vert(face[j]);

            // Convert world_coordinate to screen_coordinates
            screen_coords[j] = Vec2i((v.x + 1.) * width / 2., (v.y + 1.) * height / 2.);
            world_coords[j] = v;
        }

        // Calculate normal vector
        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        n.normalize();

        float intensity = n * light_dir;

        if (intensity > 0)
        {
            triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
        }
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}

