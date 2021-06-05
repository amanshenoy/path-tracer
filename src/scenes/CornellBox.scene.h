aspect_ratio = 1.0;
width = static_cast<int>(height * aspect_ratio);
fovy = 40.0; 

lookfrom = glm::vec3(278.0f, 278.0f, -800.0f);
lookat = glm::vec3(278.0f, 278.0f, 0.0f); 

auto red   = std::make_shared<Lambertian>(glm::vec3(0.65f, 0.05f, 0.05f));
auto white = std::make_shared<Lambertian>(glm::vec3(0.73f, 0.73f, 0.73f));
auto green = std::make_shared<Lambertian>(glm::vec3(0.12f, 0.45f, 0.15f));
auto light = std::make_shared<Emitter>(glm::vec3(15.0f));
auto glass = std::make_shared<Dielectric>(1.5, 0);
auto tex = std::make_shared<ImageTexture>("res/textures/paint.jpg");
auto other_glass = std::make_shared<Dielectric>(1.33, 0);

world.add(std::make_shared<RectangleYZ>(0, 0, 555, 555, 555, red));
world.add(std::make_shared<RectangleYZ>(0, 0, 555, 555, 0, green));
world.add(std::make_shared<RectangleXZ>(213 - 20, 227 - 20, 343 + 20, 332 + 20, 554, light));
world.add(std::make_shared<RectangleXZ>(0, 0, 555, 555, 0, white));
world.add(std::make_shared<RectangleXZ>(0, 0, 555, 555, 555, white));
world.add(std::make_shared<RectangleXY>(0, 0, 555, 555, 555, white));

std::shared_ptr<Object> Box1 = std::make_shared<Box>(glm::vec3(0), glm::vec3(165, 330, 165), white);
Box1 = std::make_shared<RotateY>(Box1, 15); 
Box1 = std::make_shared<Translate>(Box1, glm::vec3(265, 0, 295));
world.add(Box1); 

std::shared_ptr<Object> Box2 = std::make_shared<Box>(glm::vec3(0), glm::vec3(165), white);
Box2 = std::make_shared<RotateY>(Box2, -18); 
Box2 = std::make_shared<Translate>(Box2, glm::vec3(130, 0, 65));
world.add(Box2); 

world.add(std::make_shared<Sphere>(glm::vec3(200, 300, 65), glm::vec3(200, 300, 65), 0, 1, 75, glass));

auto boundary = std::make_shared<Sphere>(glm::vec3(360, 400, 250), glm::vec3(360, 400, 250), 0, 1, 50, other_glass);
world.add(boundary);
world.add(std::make_shared<ConstantMedium>(boundary, 0.9, tex));   
