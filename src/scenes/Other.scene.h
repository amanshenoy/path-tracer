aspect_ratio = 1.0;
width = static_cast<int>(height * aspect_ratio);
fovy = 25.0; 
// aperture = 1.0f / 5.6f ;

auto light = std::make_shared<core::Emitter>(glm::vec3(10));
auto light_sea_green   = std::make_shared<core::Lambertian>(glm::vec3(32.0f/255.0f, 178.0f/255.0f, 170.0f/255.0f));
auto crimson = std::make_shared<core::Metal>(glm::vec3(220.0f/255.0f, 20.0f/255.0f, 60.0f/255.0f), 1);
auto spring_green = std::make_shared<core::Lambertian>(glm::vec3(0, 255.0f/255.0f, 127.0f/255.0f));

auto steel_blue = std::make_shared<core::Lambertian>(glm::vec3(70.0f/255.0f, 30.0f/255.0f, 180.0f/255.0f));
auto white = std::make_shared<core::Lambertian>(glm::vec3(0.3f));
auto wood_tex = std::make_shared<core::ImageTexture>("res/textures/wood.jpg");
auto wood = std::make_shared<core::Lambertian>(wood_tex); 
auto wallpaper_tex = std::make_shared<core::ImageTexture>("res/textures/wallpaper.jpg");
auto chex_tex = std::make_shared<core::CheckeredTexture>(glm::vec3(160.0f/255.0f, 20.0f/255.0f, 60.0f/255.0f), glm::vec3(50.0f/255.0f, 10.0f/255.0f, 50.0f/255.0f)); 
auto chex_mat = std::make_shared<core::Lambertian>(chex_tex); 
auto wallpaper = std::make_shared<core::Lambertian>(wallpaper_tex); 
auto red_metal = std::make_shared<core::Metal>(glm::vec3(230.0f/255.0f, 230.0f/255.0f, 250.0f/255.0f), 0.7);
auto hazy_gold = std::make_shared<core::Metal>(glm::vec3(1.0, 0.49, 0.31), 0.65);
auto purple_haze_metal = std::make_shared<core::Metal>(glm::vec3(0.8, 0.2, 0.8), 0.25);
auto metal_white = std::make_shared<core::Metal>(glm::vec3(1, 1, 1), 0);
auto brown = std::make_shared<core::Lambertian>(glm::vec3(101.0f/255.0f, 67.0f/255.0f, 33.0f/255.0f));
auto haze_glass = std::make_shared<core::Dielectric>(2, 0.4);
auto new_haze_glass = std::make_shared<core::Dielectric>(2, 0.35);
auto fade_glass = std::make_shared<core::Dielectric>(1.8, 0);
auto aluminum = std::make_shared<core::Metal>(glm::vec3(0.8, 0.85, 0.88), 0.7);


auto disney = std::make_shared<core::Disney>(glm::vec3(0, 255.0f/255.0f, 127.0f/255.0f), 0, 0.4, 0.2, 0.4, false);

lookfrom = glm::vec3(0, 0, -6);
lookat = glm::vec3(0); 

aperture = 1.0f / 22.0f;
// aperture = 0.0f; 
focal_distance = glm::length(lookfrom - lookat); 

// auto table = std::make_shared<RectangleXZ>(-3, -2, 3, 2, 0, haze_glass);
// world.add(table); 
// auto table_wood = std::make_shared<RectangleXZ>(-3, -2, 3, 2, -0.1, wood);
// world.add(table_wood); 

// world.add(std::make_shared<core::Sphere>(glm::vec3(-4, 4, -4), 2.5, light)); 
// world.add(std::make_shared<Sphere>(glm::vec3(4, 8, -4), 2.5, light)); 

// lights.add(std::make_shared<core::Sphere>(glm::vec3(-4, 4, -4), 2.5, light)); 
// lights.add(std::make_shared<Sphere>(glm::vec3(4, 8, -4), 2.5, light)); 

// auto turqoise_ball = std::make_shared<core::Sphere>(glm::vec3(0.1), 0.5, disney); 
// world.add(turqoise_ball);


std::string mesh_path = "res/objects/nefertiti.obj";
auto bunny_mesh = std::make_shared<core::Mesh>(mesh_path, disney, true);

auto start_load = std::chrono::high_resolution_clock::now(); 


std::shared_ptr<core::Object> bunny = std::make_shared<optim::BVHNode>(bunny_mesh -> mesh, 0, 1);
auto end_load = std::chrono::high_resolution_clock::now();
auto load_time = std::chrono::duration_cast<std::chrono::seconds>(end_load - start_load);
std::cout << BOLDCYAN << "[ STATUS ]" << RESET << " Total object load time for " << mesh_path<<   " is ~" << load_time.count() << " seconds" << std::endl;

bunny = std::make_shared<core::RotateY>(bunny, -180);
bunny = std::make_shared<core::Translate>(bunny, glm::vec3(0, -0.7, 0));

world.add(bunny); 

