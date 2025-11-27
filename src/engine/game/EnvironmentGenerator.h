#pragma once
#include <vector>
#include <string>
#include <map>
#include <random>
#include "glm/vec3.hpp"

class World;
class EntityManager;

class EnvironmentGenerator {
public:
    EnvironmentGenerator(World& world, EntityManager& entityManager,
                         const std::map<std::string, float>& modelNamesWithWeights = {
                                 {"bush_1.obj", 5.f},
                                 {"bush_2.obj", 5.f},
                                 {"pine_tree_1.obj", 10.f},
                                 {"pine_tree_2.obj", 10.f},
                                 {"pine_tree_3.obj", 10.f},
                                 {"rock_cluster_4.obj", 1.f},
                                 {"rock_cluster_5.obj", 1.f},
                                 {"tree_stump_2.obj", 5.f}});

    int generate(float startingRadius, int startingDensity, int maxNumRings,
                 float spaceBetweenRings, glm::vec3 center);

private:
    World& world;
    EntityManager& entityManager;
    std::map<std::string, float> modelNamesWithWeights;
    std::vector<std::string> modelNames;
    std::discrete_distribution<int> weightedDist;
};
