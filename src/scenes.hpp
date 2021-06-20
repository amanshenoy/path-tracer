core::Scene GetScene(double& aspect_ratio, glm::vec3& lookfrom, glm::vec3& lookat, int& height, int& width, double& fovy, std::string name, core::Scene& lights, float& aperture, float& focal_distance, core::Background& background){
    core::Scene world; 
    if (name == "DefaultScene"){
        #include "scenes/DefaultScene.scene.h"
    }
    else if (name == "CornellBox"){
        #include "scenes/CornellBox.scene.h"
    }
    else if (name == "Other"){
        #include "scenes/Other.scene.h"
    }
    else if (name == "Statue"){
        #include "scenes/Statue.scene.h"
    }
    else if (name == "David"){
        #include "scenes/David.scene.h"
    }
    else if (name == "Thinker"){
        #include "scenes/Thinker.scene.h"
    }
    else if (name == "VaryingFuzz"){
        #include "scenes/VaryingFuzz.scene.h"
    }
    else {
        std::cout << BOLDCYAN << "[ STATUS ]" << RESET << " Not a valid scene!" << std::endl;
    }
    return world; 
}