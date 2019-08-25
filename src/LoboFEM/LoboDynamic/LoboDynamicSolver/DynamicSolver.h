#include <vector>
#include "Utils/pugixml/pugixml.hpp"

namespace Lobo
{
    class DynamicModel;
    class LoboDynamicScene;

    class DynamicSolver
    {
        public:
        DynamicSolver(LoboDynamicScene* parent_scene);
        ~DynamicSolver();

        virtual void drawImGui();

        virtual void runXMLscript(pugi::xml_node &solver_node);

        std::vector<DynamicModel*> models;
        LoboDynamicScene* parent_scene;
    };
}
