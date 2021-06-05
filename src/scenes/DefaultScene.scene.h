aspect_ratio = 5.0 / 3.0;
width = static_cast<int>(height * aspect_ratio);
fovy = 40.0; 

lookfrom = glm::vec3(-2.0, 2.0, 1.0);
lookat = glm::vec3(0.0, 0.0, -1.0); 

Scene box_floor;
auto ground = std::make_shared<Lambertian>(glm::vec3(0.48, 0.83, 0.53));

const int boxes_per_side = 16;
for (int i = 0; i < boxes_per_side; i++) {
    for (int j = 0; j < boxes_per_side; j++) {
        double w = 0.5;
        double x0 = -2 + i*w;
        double z0 = 1 - j*w;
        double y0 = -2;
        double x1 = x0 + w;
        double y1 = random_double(-2, -1);
        double z1 = z0 + w;

        box_floor.add(std::make_shared<Box>(glm::vec3(x0,y0,z0), glm::vec3(x1,y1,z1), ground));
    }
}
world.add(std::make_shared<BVHNode>(box_floor, 0, 1));

auto diffuse_green = std::make_shared<Lambertian>(glm::vec3(0.2, 0.8, 0.2));
auto hazy_gold = std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.2), 0.75);
auto red_metal = std::make_shared<Metal>(glm::vec3(0.9, 0.1, 0.2), 0);
auto purple_haze_metal = std::make_shared<Metal>(glm::vec3(0.8, 0.2, 0.8), 0.25);
auto metal_white = std::make_shared<Metal>(glm::vec3(1, 1, 1), 0);
auto glass = std::make_shared<Dielectric>(1.5, 0);
auto white_light = std::make_shared<Emitter>(glm::vec3(3));
auto blue_light = std::make_shared<Emitter>(glm::vec3(0, 0.4, 1));
auto image_tex = std::make_shared<ImageTexture>("res/textures/mickey.jpg");
auto paint = std::make_shared<ImageTexture>("res/textures/paint.jpg");
auto image_mat = std::make_shared<Lambertian>(image_tex);

Scene ball_collection; 
ball_collection.add(std::make_shared<Sphere>(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f, 0.5f, glass));
ball_collection.add(std::make_shared<Sphere>(glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(-1.0f, 0.0f, -1.0f), 0.0f, 1.0f, 0.5f, image_mat));
ball_collection.add(std::make_shared<Sphere>(glm::vec3(1.0f, 0, -1.0f), glm::vec3(1.0f, 0, -1.0f), 0.0f, 1.0f, 0.5f, blue_light));
ball_collection.add(std::make_shared<Sphere>(glm::vec3(1.0f, 0.0f, -2.0f), glm::vec3(1.0f, 0.0f, -2.0f), 0.0f, 1.0f, 0.5f, purple_haze_metal));
ball_collection.add(std::make_shared<Sphere>(glm::vec3(1.0f, 0, 0.0f), glm::vec3(1.0f, 0, 0.0f), 0.0f, 1.0f, 0.5f, red_metal));
ball_collection.add(std::make_shared<Sphere>(glm::vec3(-1.0f, 0.0f, -2.0f), glm::vec3(-1.0f, 0.0f, -2.0f), 0.0f, 1.0f, 0.5f, diffuse_green));
ball_collection.add(std::make_shared<Sphere>(glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 1.0f, 0.5f, hazy_gold));
ball_collection.add(std::make_shared<Sphere>(glm::vec3(-1, 0, 0.0f), glm::vec3(-1, 0, 0.0f), 0.0f, 1.0f, 0.5f, metal_white));

auto boundary = std::make_shared<Sphere>(glm::vec3(0), glm::vec3(0), 0, 1, 0.5, glass);
ball_collection.add(boundary);
ball_collection.add(std::make_shared<ConstantMedium>(boundary, 0.9, paint));

world.add(std::make_shared<BVHNode>(ball_collection, 0, 1)); 
world.add(std::make_shared<RectangleXZ>(-2.0f, -2.0f, 2.0f, 2.0f, 5.0f, white_light));
