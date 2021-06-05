aspect_ratio = 3.0;
width = static_cast<int>(height * aspect_ratio);
fovy = 20.0; 

auto light = std::make_shared<Emitter>(glm::vec3(8, 7, 5));
auto light_sea_green   = std::make_shared<Lambertian>(glm::vec3(32.0f/255.0f, 178.0f/255.0f, 170.0f/255.0f));
auto crimson = std::make_shared<Lambertian>(glm::vec3(220.0f/255.0f, 20.0f/255.0f, 60.0f/255.0f));
auto spring_green = std::make_shared<Lambertian>(glm::vec3(0, 255.0f/255.0f, 127.0f/255.0f));
auto steel_blue = std::make_shared<Lambertian>(glm::vec3(70.0f/255.0f, 30.0f/255.0f, 180.0f/255.0f));
auto white = std::make_shared<Lambertian>(glm::vec3(0.3f));
auto wood_tex = std::make_shared<ImageTexture>("res/textures/wood.jpg");
auto wood = std::make_shared<Lambertian>(wood_tex); 
auto wallpaper_tex = std::make_shared<ImageTexture>("res/textures/wallpaper.jpg");
auto chex_tex = std::make_shared<CheckeredTexture>(glm::vec3(160.0f/255.0f, 20.0f/255.0f, 60.0f/255.0f), glm::vec3(50.0f/255.0f, 10.0f/255.0f, 50.0f/255.0f)); 
auto chex_mat = std::make_shared<Lambertian>(chex_tex); 
auto wallpaper = std::make_shared<Lambertian>(wallpaper_tex); 
auto red_metal = std::make_shared<Metal>(glm::vec3(230.0f/255.0f, 230.0f/255.0f, 250.0f/255.0f), 0.7);
auto hazy_gold = std::make_shared<Metal>(glm::vec3(1.0, 0.49, 0.31), 0.65);
auto purple_haze_metal = std::make_shared<Metal>(glm::vec3(0.8, 0.2, 0.8), 0.25);
auto metal_white = std::make_shared<Metal>(glm::vec3(1, 1, 1), 0.4);
auto brown = std::make_shared<Lambertian>(glm::vec3(101.0f/255.0f, 67.0f/255.0f, 33.0f/255.0f));
auto haze_glass = std::make_shared<Dielectric>(2, 0.4);
auto new_haze_glass = std::make_shared<Dielectric>(2, 0.35);
auto fade_glass = std::make_shared<Dielectric>(1.8, 0);

lookfrom = glm::vec3(2, 4, -2);
lookat = glm::vec3(0, 0, 0); 

auto table = std::make_shared<RectangleXZ>(-3, -2, 3, 2, 0, haze_glass);
world.add(table); 
auto table_wood = std::make_shared<RectangleXZ>(-3, -2, 3, 2, -0.1, wood);
world.add(table_wood); 

world.add(std::make_shared<Sphere>(glm::vec3(-2, 5, -4), 2.5, light)); 
world.add(std::make_shared<Sphere>(glm::vec3(4, 8, -4), 2.5, light)); 

auto turqoise_ball = std::make_shared<Sphere>(glm::vec3(-0.5, 0.5, 0), 0.5, fade_glass); 
world.add(turqoise_ball);
world.add(std::make_shared<ConstantMedium>(turqoise_ball, 0.9, glm::vec3(0, 206.0f/255.0f, 209.0f/255.0f)));  

auto teapot_mesh = std::make_shared<Mesh>("objects/teapot.obj", red_metal, false);
std::shared_ptr<Object> teapot = std::make_shared<BVHNode>(teapot_mesh -> mesh, 0, 1);
teapot = std::make_shared<RotateY>(teapot, 165);
teapot = std::make_shared<Translate>(teapot, glm::vec3(1, 0, 0.5));
world.add(teapot); 

auto bunny_mesh = std::make_shared<Mesh>("objects/bunny.obj", hazy_gold, true);
std::shared_ptr<Object> bunny = std::make_shared<BVHNode>(bunny_mesh -> mesh, 0, 1);
bunny = std::make_shared<RotateY>(bunny, 120);
bunny = std::make_shared<Translate>(bunny, glm::vec3(-0.5, 0, -1));
world.add(bunny); 