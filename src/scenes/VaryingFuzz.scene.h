aspect_ratio = 2.5;
width = static_cast<int>(height * aspect_ratio);
fovy = 45.0;

lookfrom = glm::vec3(0, 130, -200);
lookat = glm::vec3(0); 

auto light = std::make_shared<Emitter>(glm::vec3(5.0f));
auto white = std::make_shared<Lambertian>(glm::vec3(0.3f));
auto metal_white = std::make_shared<Metal>(glm::vec3(1, 1, 1), 0);
auto glass = std::make_shared<Dielectric>(1.5, 0);

Scene balls; 
balls.add(std::make_shared<Sphere>(glm::vec3(75 * -2, 0, 0), 35, std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.2), 0.0f)));
balls.add(std::make_shared<Sphere>(glm::vec3(75 * -1, 0, 0), 35, std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.2), 0.25f)));
balls.add(std::make_shared<Sphere>(glm::vec3(0), 35, std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.2), 0.5f)));
balls.add(std::make_shared<Sphere>(glm::vec3(75 * 1, 0, 0), 35, std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.2), 0.75f)));
balls.add(std::make_shared<Sphere>(glm::vec3(75 * 2, 0, 0), 35, std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.2), 1.0f)));
balls.add(std::make_shared<Box>(glm::vec3(75 * -3, -45, -35), glm::vec3(75 * 3, -38, 35), white));
world.add(std::make_shared<BVHNode>(balls, 0, 1)); 

world.add(std::make_shared<RectangleXY>(-300, -100, 300, 100, -200, light));
world.add(std::make_shared<Box>(glm::vec3(75 * -3, -45 + 150, -35), glm::vec3(75 * 3, -38 + 150, 35), white));

Scene lights; 
lights.add(std::make_shared<Sphere>(glm::vec3(75 * -3, 150, 0), 35, light));
world.add(std::make_shared<BVHNode>(lights, 0, 1)); 