aspect_ratio = 8.0 / 3.0;
width = static_cast<int>(height * aspect_ratio);
fovy = 20.0; 

lookfrom = glm::vec3(0, 0.25, -8.25);
lookat = glm::vec3(0, 0.125, 0); 

// Materials
auto light_sea_green   = std::make_shared<Lambertian>(glm::vec3(32.0f/255.0f, 178.0f/255.0f, 170.0f/255.0f));
auto crimson = std::make_shared<Lambertian>(glm::vec3(220/255, 20.0f/255.0f, 60.0f/255.0f));
auto spring_green = std::make_shared<Lambertian>(glm::vec3(0, 255.0f/255.0f, 127.0f/255.0f));
auto steel_blue = std::make_shared<Lambertian>(glm::vec3(70.0f/255.0f, 30.0f/255.0f, 180.0f/255.0f));
auto white = std::make_shared<Lambertian>(glm::vec3(0.3f));

auto light = std::make_shared<Emitter>(glm::vec3(7.5f));
auto orange_light = std::make_shared<Emitter>(glm::vec3(255.0f/255.0f, 140.0f/255.0f, 0));        
auto yellow_light = std::make_shared<Emitter>(glm::vec3(10, 8, 4));
auto white_light = std::make_shared<Emitter>(glm::vec3(3));
auto blue_light = std::make_shared<Emitter>(glm::vec3(0, 0.4, 1));
auto pink_light = std::make_shared<Emitter>(glm::vec3(255.0f/255.0f, 192.0f/255.0f, 203.0f/255.0f));
auto gold_light = std::make_shared<Emitter>(glm::vec3(255.0f/255.0f, 215.0f/255.0f,0));

auto hazy_gold = std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.2), 0.75);
auto red_metal = std::make_shared<Metal>(glm::vec3(0.9, 0.1, 0.2), 0.2);
auto purple_haze_metal = std::make_shared<Metal>(glm::vec3(0.8, 0.2, 0.8), 0.7);
auto metal_white = std::make_shared<Metal>(glm::vec3(1, 1, 1), 0.8);

auto glass = std::make_shared<Dielectric>(1.5, 0);
auto fade_glass = std::make_shared<Dielectric>(1.4, 0);

auto mickey_tex = std::make_shared<ImageTexture>("res/textures/mickey.jpg");
auto paint_tex = std::make_shared<ImageTexture>("res/textures/paint_2.jpg");
auto marble_1_tex = std::make_shared<ImageTexture>("res/textures/marble1.jpg");
auto marble_2_tex = std::make_shared<ImageTexture>("res/textures/marble2.jpg");
auto billiards_tex = std::make_shared<ImageTexture>("res/textures/billiards_ball.jpg");

auto mickey = std::make_shared<Lambertian>(mickey_tex);
auto marble_1 = std::make_shared<Lambertian>(marble_1_tex);
auto marble_2 = std::make_shared<Lambertian>(marble_2_tex);
auto paint = std::make_shared<Lambertian>(paint_tex);

// The balls 
//////////////////////////////////////////////////////////////
world.add(std::make_shared<RectangleXY>(-0.5, -3, 0.5, 3, -10, yellow_light));

world.add(std::make_shared<RectangleXY>(-2.5, -3, -1.5, 3, -10, yellow_light));
world.add(std::make_shared<RectangleXY>(-4.5, -3, -3.5, 3, -10, yellow_light));
world.add(std::make_shared<RectangleXY>(-6.5, -3, -5.5, 3, -10, yellow_light));
world.add(std::make_shared<RectangleXY>(-8.5, -3, -7.5, 3, -10, yellow_light));

world.add(std::make_shared<RectangleXY>(1.5, -3, 2.5, 3, -10, yellow_light));
world.add(std::make_shared<RectangleXY>(3.5, -3, 4.5, 3, -10, yellow_light));
world.add(std::make_shared<RectangleXY>(5.5, -3, 6.5, 3, -10, yellow_light));
world.add(std::make_shared<RectangleXY>(7.5, -3, 8.5, 3, -10, yellow_light));

// Bottom row farthest from camera
world.add(std::make_shared<Sphere>(glm::vec3(-3, -1, 1), 1, crimson));
world.add(std::make_shared<Sphere>(glm::vec3(1, -1, 1), 1, mickey));
world.add(std::make_shared<Sphere>(glm::vec3(-1, -1, 1), 1, spring_green));

world.add(std::make_shared<Sphere>(glm::vec3(-3, 0, 1), 1, steel_blue));
world.add(std::make_shared<Sphere>(glm::vec3(1, 0,  1), 1, blue_light));
world.add(std::make_shared<Sphere>(glm::vec3(2, sqrt(3), 1), 1, crimson));
world.add(std::make_shared<Sphere>(glm::vec3(-2, sqrt(3), 1), 1, light_sea_green));

// Middle row 
auto red_ball = std::make_shared<Sphere>(glm::vec3(3, 1, 0), 1, fade_glass); 
world.add(red_ball);
world.add(std::make_shared<ConstantMedium>(red_ball, 0.9, glm::vec3(220.0f/255.0f, 20.0f/255.0f, 60.0f/255.0f)));  

auto turqoise_ball = std::make_shared<Sphere>(glm::vec3(-3, 1, 0), 1, fade_glass); 
world.add(turqoise_ball);
world.add(std::make_shared<ConstantMedium>(turqoise_ball, 0.9, glm::vec3(0, 206.0f/255.0f, 209.0f/255.0f)));  

world.add(std::make_shared<Sphere>(glm::vec3(1, 1, 0), 1, gold_light));
world.add(std::make_shared<Sphere>(glm::vec3(-1, 1, 0), 1, metal_white));

auto billiards_ball = std::make_shared<Sphere>(glm::vec3(0, 1.0 - sqrt(3), 0), 1, fade_glass); 
world.add(billiards_ball);
std::shared_ptr<Object> medium = std::make_shared<ConstantMedium>(billiards_ball, 0.9, billiards_tex);
medium = std::make_shared<RotateY>(medium, -45); 
world.add(medium);  


world.add(std::make_shared<Sphere>(glm::vec3(2, 1.0 - sqrt(3), 0), 1, purple_haze_metal));
auto marble_boundary_0 = std::make_shared<Sphere>(glm::vec3(-2, 1.0 - sqrt(3), 0), 1, fade_glass); 
world.add(marble_boundary_0);
world.add(std::make_shared<ConstantMedium>(marble_boundary_0, 0.9, paint_tex));           
world.add(std::make_shared<Sphere>(glm::vec3(4, 1.0 - sqrt(3), 0), 1, pink_light));
world.add(std::make_shared<Sphere>(glm::vec3(-4, 1.0 - sqrt(3), 0), 1, orange_light));

// Row closest to camera
auto marble_boundary_1 = std::make_shared<Sphere>(glm::vec3(2, 0, -1.0), 0.9, fade_glass);
world.add(marble_boundary_1); 
world.add(std::make_shared<ConstantMedium>(marble_boundary_1, 0.9, marble_1_tex));   

auto marble_boundary_2 = std::make_shared<Sphere>(glm::vec3(-1, 0, -1.0), 0.9, fade_glass); 
world.add(marble_boundary_2);
world.add(std::make_shared<ConstantMedium>(marble_boundary_2, 0.9, marble_2_tex));   

auto marble_boundary_3 = std::make_shared<Sphere>(glm::vec3(0, sqrt(3), -1), 1, fade_glass); 
world.add(marble_boundary_3);
world.add(std::make_shared<ConstantMedium>(marble_boundary_3, 0.9, paint_tex));   

world.add(std::make_shared<Sphere>(glm::vec3(glm::vec3(3, - sqrt(3) - 0.1, -1.0)), 0.9, red_metal));

auto ico_mesh = std::make_shared<Mesh>("objects/ico.obj", hazy_gold);
std::shared_ptr<Object> ico = std::make_shared<BVHNode>(ico_mesh -> mesh, 0, 1);
ico = std::make_shared<RotateY>(ico, 165);
ico = std::make_shared<Translate>(ico, glm::vec3(0.35 - 0.3, -0.25 + 0.3, -1));
world.add(ico); 
