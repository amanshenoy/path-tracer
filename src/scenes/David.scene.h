aspect_ratio = 1.0;
width = static_cast<int>(height * aspect_ratio);
fovy = 25.0; 

auto light = std::make_shared<core::Emitter>(glm::vec3(1));
auto fade_glass = std::make_shared<core::Dielectric>(1.8, 0.75);

auto granite = std::make_shared<core::Disney>(glm::vec3(0.4), 0.5, 0.5, 0, 0, false);
granite -> surface_params.transmittanceColor = glm::vec3(1);
granite -> surface_params.sheen = 0;
granite -> surface_params.sheenTint= 0;
granite -> surface_params.diffTrans= 0;
granite -> surface_params.flatness= 1;
granite -> surface_params.roughness= 0;
granite -> surface_params.scatterDistance= 1;

auto table_top = std::make_shared<core::Disney>(glm::vec3(0.2), 0.4, 0.6, 0, 0, true);
table_top -> surface_params.diffTrans = 0.5; 

lookfrom = glm::vec3(2, 2.5, -5);
lookat = glm::vec3(2, 1.9, 0); 

aperture = 1.0f / 22.0f;
// aperture = 0.0f; 
focal_distance = glm::length(lookfrom - lookat); 

std::shared_ptr<core::Object> light_bulb = (std::make_shared<core::RectangleXZ>(-3, -3, 5, 0, 8, light));
light_bulb = std::make_shared<core::RotateY>(light_bulb, 30); 
world.add(light_bulb); 

std::shared_ptr<core::Object> box = std::make_shared<core::Box>(glm::vec3(-1.9, .9, -1), glm::vec3(4, 1, 1), table_top);
// box = std::make_shared<core::RotateY>(box, 30); 
box = std::make_shared<core::Translate>(box, glm::vec3(2, 0, 0));
world.add(box); 

std::shared_ptr<core::Object> box2 = std::make_shared<core::Box>(glm::vec3(-1.8, 0, -0.9), glm::vec3(3.9, .9, 0.9), fade_glass);
// box2 = std::make_shared<core::RotateY>(box2, 30); 
box2 = std::make_shared<core::Translate>(box2, glm::vec3(2, 0, 0));
world.add(box2); 

std::string mesh_path = "res/data/david.obj";
auto bunny_mesh = std::make_shared<core::Mesh>(mesh_path, granite, true);
auto start_load = std::chrono::high_resolution_clock::now(); 
std::shared_ptr<core::Object> bunny = std::make_shared<optim::BVHNode>(bunny_mesh -> mesh, 0, 1);
auto end_load = std::chrono::high_resolution_clock::now();
auto load_time = std::chrono::duration_cast<std::chrono::seconds>(end_load - start_load);
std::cout << BOLDCYAN << "[ STATUS ]" << RESET << " Total object load time for " << mesh_path<<   " is ~" << load_time.count() << " seconds" << std::endl;

bunny = std::make_shared<core::RotateY>(bunny, 65);
bunny = std::make_shared<core::Translate>(bunny, glm::vec3(2, 1, 0));
world.add(bunny); 