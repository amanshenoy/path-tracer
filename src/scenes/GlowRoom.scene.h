aspect_ratio = 4.0 / 3.0;
width = static_cast<int>(height * aspect_ratio);
fovy = 40.0; 

lookfrom = glm::vec3(278.0f, 278.0f, -1000.0f);
lookat = glm::vec3(278.0f, 278.0f, 0.0f); 

auto white = std::make_shared<Lambertian>(glm::vec3(0.73f, 0.73f, 0.73f));
auto grey = std::make_shared<Lambertian>(glm::vec3(0.23f, 0.23f, 0.23f));
auto light = std::make_shared<Emitter>(glm::vec3(15.0f));
auto cyan_light = std::make_shared<Emitter>(glm::vec3(0.0f, 15.0f, 15.0f));
auto glass = std::make_shared<Dielectric>(1.5, 0);

auto tex = std::make_shared<ImageTexture>("res/textures/wood.jpeg");
auto tex2 = std::make_shared<ImageTexture>("res/textures/dark.jpg");
auto tex3 = std::make_shared<ImageTexture>("res/textures/marble.jpg");

auto wood = std::make_shared<Lambertian>(tex); 
auto darker_wood = std::make_shared<Lambertian>(tex2); 
auto marble = std::make_shared<Lambertian>(tex3);
auto other_glass = std::make_shared<Dielectric>(1.33, 0);

world.add(std::make_shared<RectangleYZ>(0, -200, 555, 555, 655, grey));
world.add(std::make_shared<RectangleYZ>(0, -200, 555, 555, -100, grey));

world.add(std::make_shared<RectangleXZ>(-100, 200, 655, 350, 554, light));
world.add(std::make_shared<RectangleXZ>(-100, 0, 655, 20, 554, light));
world.add(std::make_shared<RectangleXZ>(-100, -60, 655, -40, 554, light));

world.add(std::make_shared<RectangleYZ>(0, -60, 555, -40, -100, cyan_light));
world.add(std::make_shared<RectangleYZ>(0, 0, 555, 20, -100, cyan_light));
world.add(std::make_shared<RectangleYZ>(0, -60, 555, -40, 655, cyan_light));
world.add(std::make_shared<RectangleYZ>(0, 0, 555, 20, 655, cyan_light));

world.add(std::make_shared<RectangleXZ>(-100, -200, 655, 555, 0, marble));
world.add(std::make_shared<RectangleXZ>(-100, -200, 655, 555, 555, white));

world.add(std::make_shared<RectangleXY>(-100, 0, 655, 555, 555, grey));

std::shared_ptr<Object> Box1 = std::make_shared<Box>(glm::vec3(0), glm::vec3(165, 330, 165), darker_wood);
Box1 = std::make_shared<RotateY>(Box1, 45); 
Box1 = std::make_shared<Translate>(Box1, glm::vec3(265, 0, 295));
world.add(Box1); 

std::shared_ptr<Object> Box2 = std::make_shared<Box>(glm::vec3(0), glm::vec3(165, 225, 165), wood);
Box2 = std::make_shared<RotateY>(Box2, 22); 
Box2 = std::make_shared<Translate>(Box2, glm::vec3(90, 0, 45));
world.add(Box2); 

world.add(std::make_shared<Sphere>(glm::vec3(200, 300, 45), 75, glass));

auto boundary = std::make_shared<Sphere>(glm::vec3(360, 400, 250), glm::vec3(360, 400, 250), 0, 1, 70, other_glass);
world.add(boundary);
world.add(std::make_shared<ConstantMedium>(boundary, 0.9, glm::vec3(0.2, 0.4, 0.9)));   
