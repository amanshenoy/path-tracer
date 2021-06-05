Scene GetScene(double& aspect_ratio, glm::vec3& lookfrom, glm::vec3& lookat, int& height, int& width, double& fovy, std::string name){
    Scene world; 
    if (name == "DefaultScene"){
        #include "scenes/DefaultScene.scene.h"
    }
    else if (name == "CornellBox"){
        #include "scenes/CornellBox.scene.h"
    }
    else if (name == "GlowRoom"){
        #include "scenes/GlowRoom.scene.h"
    }
    else if (name == "VaryingFuzz"){
        #include "scenes/VaryingFuzz.scene.h"
    }
    else if (name == "Marbles"){
        #include "scenes/Marbles.scene.h"
    }
    else if (name == "Test"){
        #include "scenes/Test.scene.h"
    }
    else if (name == "Other"){
        #include "scenes/Other.scene.h"
    }
    else {
        std::cout << BOLDCYAN << "[ STATUS ]" << RESET << " Not a valid scene!" << std::endl;
    }
    return world; 
}
