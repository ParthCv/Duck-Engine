#pragma once

#include <memory>
#include <string>

class StaticMesh;
class Material;

struct StaticMeshComponent
{
    std::shared_ptr<StaticMesh> Mesh;
    std::shared_ptr<Material> material;
    bool bIsVisible = true;
    bool bTicks = true;
};