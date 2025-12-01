#include "EnvironmentGenerator.h"
#include "ecs/EnvironmentEntity.h"
#include "../src/engine/ecs/system/EntityManager.h"
#include "GameUtils.h"

EnvironmentGenerator::EnvironmentGenerator(World& world, EntityManager& entityManager,
                                           const std::map<std::string, float>& modelNamesWithWeights)
        : world(world), entityManager(entityManager), modelNamesWithWeights(modelNamesWithWeights)
{
    std::vector<float> weights;
    for (const auto& Pair : modelNamesWithWeights)
    {
        modelNames.push_back(Pair.first);
        weights.push_back(Pair.second);
    }
    weightedDist = std::discrete_distribution<int>(weights.begin(), weights.end());
}

int EnvironmentGenerator::generate(float startingRadius, int startingDensity, int maxNumRings, float spaceBetweenRings, glm::vec3 center)
{
    if (modelNamesWithWeights.empty()) return 0;

    // random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    int spawnedNumEntities = 0;
    float currentRadius = startingRadius;
    for (int i = 0; i < maxNumRings; ++i) {
        std::vector<glm::vec3> ringPoints = GenerateRingPoints(center, currentRadius, (i + 1) * startingDensity, 1.f, 0.f);
        for (auto& pos : ringPoints) {
            const std::string& randomModelName = modelNames[weightedDist(gen)];
            entityManager.CreateEntityOfType<EnvironmentEntity>(world, pos, randomModelName);
            ++spawnedNumEntities;
        }
        currentRadius += spaceBetweenRings;
    }

    return spawnedNumEntities;
}