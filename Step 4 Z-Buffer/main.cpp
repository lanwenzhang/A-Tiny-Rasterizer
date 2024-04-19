#include <vector>
#include <cmath>
#include <cstdlib>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
Model* model = NULL;
const int width = 800;
const int height = 800;


// Calculate barycentric coordinates
Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P)
{
    Vec3f s[2];
    // Calculate [AB, AC, PA] x, y, z coordinates
    for (int i = 2; i--;)
    {
        s[i][0] = C[i] - A[i];
        s[i][1] = B[i] - A[i];
        s[i][2] = A[i] - P[i];
    }
    
    // u perpendicular to [ABx, ACx, PAx] and [ABy, ACy, PAy]
    Vec3f u = cross(s[0], s[1]);
    if (std::abs(u[2]) > 1e-2)
     return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
    return Vec3f(-1, 1, 1);
 
}


// Shade a triangle with bounding box method
void triangle(Vec3f* pts, float* zbuffer, TGAImage& image, TGAColor color)
{
    // Give initial values for bounding box
    Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

    // Clamp the image
    Vec2f clamp(image.get_width() - 1, image.get_height() - 1);

    // Draw bounding box for each triangle
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::max(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }

    Vec3f P;
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
    {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
        {
            // Generate Determine vector(1-u-v, u, v)
            Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);

            // If P is outside the triangle, end the loop and check the next value
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;

            // If P is inside the triangle, initial the z-value
            P.z = 0;

            // Calculate z-buffer
            for (int i = 0; i < 3; i++)
            {
                P.z += pts[i][2] * bc_screen[i];
            }
            if (zbuffer[int(P.x + P.y * width)] < P.z)
            {
                zbuffer[int(P.x + P.y * width)] = P.z;
                image.set(P.x, P.y, color);
            }
        }
    }
}


    Vec3f world2screen(Vec3f v)
    {
        return Vec3f(int((v.x + 1.) * width / 2. + .5), int((v.y + 1.) * height / 2. + .5), v.z);
    }

int main(int argc, char** argv) 
{
    // Load the model
    if (2 == argc) {
        model = new Model(argv[1]);
    }
    else {
        model = new Model("D:/Career/Knowledge/Graphics_Programming/TinyRasterizer/obj/african_head.obj");
    }
    
    // Create an arry point to the zbuffer value
    float* zbuffer = new float[width * height];

    // Initial the z-buffer value
    for (int i = width * height; i--;)
    {
        zbuffer[i] = -std::numeric_limits<float>::max();
    }

    // Construct the tgaimage
    TGAImage image(width, height, TGAImage::RGB);

    // Define the direction of light
    Vec3f light_dir(0, 0, -1);

    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        Vec3f screen_coords[3];
        Vec3f world_coords[3];
        for (int j = 0; j < 3; j++)
        {

            // Convert world_coordinate to screen_coordinates
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = world2screen(model->vert(face[j]));
            world_coords[j] = v;
        }

        // Calculate normal vector
        Vec3f n = cross((world_coords[2] - world_coords[0]), (world_coords[1] - world_coords[0]));
        n.normalize();

        float intensity = n * light_dir;

        if (intensity > 0)
        {
            triangle(screen_coords, zbuffer, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
        }
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}
