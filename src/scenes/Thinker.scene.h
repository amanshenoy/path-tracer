aspect_ratio = 1.0;
width = static_cast<int>(height * aspect_ratio);
fovy = 25.0; 

auto light = std::make_shared<core::Emitter>(glm::vec3(4));
auto light_sea_green   = std::make_shared<core::Metal>(glm::vec3(32.0f/255.0f, 178.0f/255.0f, 170.0f/255.0f), 0.2);
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
auto metal_white = std::make_shared<core::Metal>(glm::vec3(1, 1, 1), 0.1);
auto brown = std::make_shared<core::Lambertian>(glm::vec3(101.0f/255.0f, 67.0f/255.0f, 33.0f/255.0f));
auto haze_glass = std::make_shared<core::Dielectric>(2, 0.4);
auto new_haze_glass = std::make_shared<core::Dielectric>(2, 0.35);
auto fade_glass = std::make_shared<core::Dielectric>(1.8, 0.75);
auto aluminum = std::make_shared<core::Metal>(glm::vec3(0.8, 0.85, 0.88), 0.7);


auto granite = std::make_shared<core::Disney>(glm::vec3(1), 1, 0, 0, 0, false);
// granite -> surface_params.transmittanceColor = glm::vec3(1);
// granite -> surface_params.sheen = 0.1;
// granite -> surface_params.sheenTint= 0;
granite -> surface_params.diffTrans= 0;
// granite -> surface_params.flatness= 1;
// granite -> surface_params.roughness= 0;
// granite -> surface_params.scatterDistance= 1;

auto tint = std::make_shared<core::Disney>(glm::vec3(200.0f/255.0f, 0, 0), 0.8, 0.0, 0, 0.2, false);
tint -> surface_params.transmittanceColor = glm::vec3(0, 1, 0);
// tint -> surface_params.sheen = 0;
// tint -> surface_params.sheenTint= 0;
// tint -> surface_params.diffTrans= 0.1;
// tint -> surface_params.flatness= 0.3;
// tint -> surface_params.roughness= 0;
// tint -> surface_params.scatterDistance= 0.2;

// tint -> surface_params.anisotropic= 0.8;
// tint -> surface_params.specularTint= 0.8;
// tint -> surface_params.specTrans= 1;
tint -> surface_params.ior = 1.5;
tint -> surface_params.relativeIOR = 1/1.5;

auto table_top = std::make_shared<core::Disney>(glm::vec3(0.05), 0.4, 0.6, 0, 0, true);
table_top -> surface_params.diffTrans = 0.5; 

lookfrom = glm::vec3(2, 2.5, -5);
lookat = glm::vec3(2, 1.9, 0); 

aperture = 1.0f / 22.0f;
// aperture = 0.0f; 
focal_distance = glm::length(lookfrom - lookat); 

std::shared_ptr<core::Object> light_bulb = (std::make_shared<core::RectangleXZ>(-3, -3, 5, 0, 8, light));
// light_bulb = std::make_shared<core::RotateY>(light_bulb, 30); 
world.add(light_bulb); 

std::shared_ptr<core::Object> box = std::make_shared<core::Box>(glm::vec3(-1.9, .9, -1), glm::vec3(4, 1, 1), table_top);
// box = std::make_shared<core::RotateY>(box, 30); 
box = std::make_shared<core::Translate>(box, glm::vec3(2, 0, 0));
world.add(box); 

std::shared_ptr<core::Object> box2 = std::make_shared<core::Box>(glm::vec3(-1.8, 0, -0.9), glm::vec3(3.9, .9, 0.9), fade_glass);
// box2 = std::make_shared<core::RotateY>(box2, 30); 
box2 = std::make_shared<core::Translate>(box2, glm::vec3(2, 0, 0));
world.add(box2); 


std::string mesh_path = "res/data/thinker.obj";
auto bunny_mesh = std::make_shared<core::Mesh>(mesh_path, hazy_gold, true);
auto start_load = std::chrono::high_resolution_clock::now(); 
std::shared_ptr<core::Object> bunny = std::make_shared<optim::BVHNode>(bunny_mesh -> mesh, 0, 1);
auto end_load = std::chrono::high_resolution_clock::now();
auto load_time = std::chrono::duration_cast<std::chrono::seconds>(end_load - start_load);
std::cout << BOLDCYAN << "[ STATUS ]" << RESET << " Total object load time for " << mesh_path<<   " is ~" << load_time.count() << " seconds" << std::endl;

bunny = std::make_shared<core::RotateY>(bunny, 165);
bunny = std::make_shared<core::Translate>(bunny, glm::vec3(2, 1, 0));
world.add(bunny); 


// mesh_path = "res/data/happy.obj";
// auto happy_mesh = std::make_shared<core::Mesh>(mesh_path, hazy_gold, true);
// start_load = std::chrono::high_resolution_clock::now(); 
// std::shared_ptr<core::Object> happy = std::make_shared<optim::BVHNode>(happy_mesh -> mesh, 0, 1);
// end_load = std::chrono::high_resolution_clock::now();
// load_time = std::chrono::duration_cast<std::chrono::seconds>(end_load - start_load);
// std::cout << BOLDCYAN << "[ STATUS ]" << RESET << " Total object load time for " << mesh_path<<   " is ~" << load_time.count() << " seconds" << std::endl;

// happy = std::make_shared<core::RotateY>(happy, 220);
// happy = std::make_shared<core::Translate>(happy, glm::vec3(1.2, 1, -1));
// world.add(happy); 
