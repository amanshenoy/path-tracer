aspect_ratio = 3;
width = static_cast<int>(height * aspect_ratio);
fovy = 25.0;

lookfrom = glm::vec3(0, 0, -3);
lookat = glm::vec3(0); 

aperture = 1.0f / 22.0f;
focal_distance = glm::length(lookfrom - lookat); 

world.add(std::make_shared<core::Sphere>(glm::vec3(.75 * -2, 0, 0), .35, std::make_shared<core::Metal>(glm::vec3(1), 0.0f)));
world.add(std::make_shared<core::Sphere>(glm::vec3(.75 * -1, 0, 0), .35, std::make_shared<core::Metal>(glm::vec3(1), 0.1f)));
world.add(std::make_shared<core::Sphere>(glm::vec3(0), .35, std::make_shared<core::Metal>(glm::vec3(1), 0.2f)));
world.add(std::make_shared<core::Sphere>(glm::vec3(.75 * 1, 0, 0), .35, std::make_shared<core::Metal>(glm::vec3(1), 0.3f)));
world.add(std::make_shared<core::Sphere>(glm::vec3(.75 * 2, 0, 0), .35, std::make_shared<core::Metal>(glm::vec3(1), 0.4f)));