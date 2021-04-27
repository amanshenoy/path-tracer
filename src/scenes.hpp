Scene GetScene(double& aspect_ratio, glm::vec3& lookfrom, glm::vec3& lookat, int& height, int& width, double& fovy, std::string name){

    Scene objects; 

    if (name == "DefaultScene"){
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
        objects.add(std::make_shared<BVHNode>(box_floor, 0, 1));

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

        objects.add(std::make_shared<BVHNode>(ball_collection, 0, 1)); 
        objects.add(std::make_shared<RectangleXZ>(-2.0f, -2.0f, 2.0f, 2.0f, 5.0f, white_light));
    }

    else if (name == "CornellBox"){
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

        objects.add(std::make_shared<RectangleYZ>(0, 0, 555, 555, 555, red));
        objects.add(std::make_shared<RectangleYZ>(0, 0, 555, 555, 0, green));
        objects.add(std::make_shared<RectangleXZ>(213 - 20, 227 - 20, 343 + 20, 332 + 20, 554, light));
        objects.add(std::make_shared<RectangleXZ>(0, 0, 555, 555, 0, white));
        objects.add(std::make_shared<RectangleXZ>(0, 0, 555, 555, 555, white));
        objects.add(std::make_shared<RectangleXY>(0, 0, 555, 555, 555, white));

        std::shared_ptr<Object> Box1 = std::make_shared<Box>(glm::vec3(0), glm::vec3(165, 330, 165), white);
        Box1 = std::make_shared<RotateY>(Box1, 15); 
        Box1 = std::make_shared<Translate>(Box1, glm::vec3(265, 0, 295));
        objects.add(Box1); 
        
        std::shared_ptr<Object> Box2 = std::make_shared<Box>(glm::vec3(0), glm::vec3(165), white);
        Box2 = std::make_shared<RotateY>(Box2, -18); 
        Box2 = std::make_shared<Translate>(Box2, glm::vec3(130, 0, 65));
        objects.add(Box2); 
        
        objects.add(std::make_shared<Sphere>(glm::vec3(200, 300, 65), glm::vec3(200, 300, 65), 0, 1, 75, glass));

        auto boundary = std::make_shared<Sphere>(glm::vec3(360, 400, 250), glm::vec3(360, 400, 250), 0, 1, 50, other_glass);
        objects.add(boundary);
        objects.add(std::make_shared<ConstantMedium>(boundary, 0.9, tex));   
    }

    else if (name == "GlowRoom"){
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

        objects.add(std::make_shared<RectangleYZ>(0, -200, 555, 555, 655, grey));
        objects.add(std::make_shared<RectangleYZ>(0, -200, 555, 555, -100, grey));
        
        objects.add(std::make_shared<RectangleXZ>(-100, 200, 655, 350, 554, light));
        objects.add(std::make_shared<RectangleXZ>(-100, 0, 655, 20, 554, light));
        objects.add(std::make_shared<RectangleXZ>(-100, -60, 655, -40, 554, light));

        objects.add(std::make_shared<RectangleYZ>(0, -60, 555, -40, -100, cyan_light));
        objects.add(std::make_shared<RectangleYZ>(0, 0, 555, 20, -100, cyan_light));
        objects.add(std::make_shared<RectangleYZ>(0, -60, 555, -40, 655, cyan_light));
        objects.add(std::make_shared<RectangleYZ>(0, 0, 555, 20, 655, cyan_light));

        objects.add(std::make_shared<RectangleXZ>(-100, -200, 655, 555, 0, marble));
        objects.add(std::make_shared<RectangleXZ>(-100, -200, 655, 555, 555, white));

        objects.add(std::make_shared<RectangleXY>(-100, 0, 655, 555, 555, grey));

        std::shared_ptr<Object> Box1 = std::make_shared<Box>(glm::vec3(0), glm::vec3(165, 330, 165), darker_wood);
        Box1 = std::make_shared<RotateY>(Box1, 45); 
        Box1 = std::make_shared<Translate>(Box1, glm::vec3(265, 0, 295));
        objects.add(Box1); 
        
        std::shared_ptr<Object> Box2 = std::make_shared<Box>(glm::vec3(0), glm::vec3(165, 225, 165), wood);
        Box2 = std::make_shared<RotateY>(Box2, 22); 
        Box2 = std::make_shared<Translate>(Box2, glm::vec3(90, 0, 45));
        objects.add(Box2); 
        
        objects.add(std::make_shared<Sphere>(glm::vec3(200, 300, 45), 75, glass));

        auto boundary = std::make_shared<Sphere>(glm::vec3(360, 400, 250), glm::vec3(360, 400, 250), 0, 1, 70, other_glass);
        objects.add(boundary);
        objects.add(std::make_shared<ConstantMedium>(boundary, 0.9, glm::vec3(0.2, 0.4, 0.9)));   

    }
    else if (name == "VaryingFuzz"){
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
        objects.add(std::make_shared<BVHNode>(balls, 0, 1)); 

        objects.add(std::make_shared<RectangleXY>(-300, -100, 300, 100, -200, light));
        objects.add(std::make_shared<Box>(glm::vec3(75 * -3, -45 + 150, -35), glm::vec3(75 * 3, -38 + 150, 35), white));

        Scene lights; 
        lights.add(std::make_shared<Sphere>(glm::vec3(75 * -3, 150, 0), 35, light));
        objects.add(std::make_shared<BVHNode>(lights, 0, 1)); 
    }
    else {
        std::cout << BOLDCYAN << "[ STATUS ]" << RESET << " Not a valid scene!" << std::endl;
    }
    return objects; 
}
