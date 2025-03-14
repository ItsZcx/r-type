/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Systems
*/

#include "ecs/Systems.hpp"

void check_collision(client::Registry &registry, std::size_t projectile_index, const sf::Sprite &projectile,
                     const client::EntityType target)
{
    auto &type = registry.get_components<client::components::type>();
    auto &drawable = registry.get_components<client::components::drawable>();

    for (std::size_t entity = 0; entity < drawable.size(); entity++)
    {
        if (drawable[entity] && type[entity] && type[entity]->type == target)
        {
            if (drawable[entity]->sprite.getGlobalBounds().intersects(projectile.getGlobalBounds()))
            {
                // registry.kill_entity(client::Entity(entity));
                // registry.kill_entity(client::Entity(projectile_index));
            }
        }
    }
}

void client::render_system(client::Registry &registry, sf::RenderWindow &window)
{
    window.clear();
    sf::Clock clock;
    clock.restart();

    auto &parallaxLayers = registry.get_parallax_layers();
    for (const auto &layer : parallaxLayers)
    {
        layer.render(window);
    }

    auto &drawables = registry.get_components<components::drawable>();
    auto &positions = registry.get_components<components::position>();
    auto &types = registry.get_components<components::type>();

    for (std::size_t entity = 0; entity < drawables.size(); entity++)
    {
        if (drawables[entity] && positions[entity])
        {
            if (types[entity]->type == EntityType::BOSS)
            {
                drawables[entity]->sprite.setPosition(positions[entity]->x - 328.5, positions[entity]->y - 50);
            } else
            {
                drawables[entity]->sprite.setPosition(positions[entity]->x, positions[entity]->y);
            }
            window.draw(drawables[entity]->sprite);
        }
    }
    window.draw(registry.get_health_bar_box());
    window.draw(registry.get_health_bar());
    window.draw(registry.get_heart());

    window.display();
    std::cout << "Render time: " << clock.getElapsedTime().asMilliseconds() << "ms" << std::endl;
}

void client::movement_system(client::Registry &registry, float &deltaTime)
{
    sf::Clock clock;
    clock.restart();
    auto &positions = registry.get_components<components::position>();
    auto &velocities = registry.get_components<components::velocity>();

    for (std::size_t entity = 0; entity < positions.size(); entity++)
    {
        if (positions[entity] && velocities[entity])
        {
            // std::cout << "Entity: " << entity << std::endl;
            // std::cout << "Velocity x: " << velocities[entity]->x * deltaTime << " Velocity y: " << velocities[entity]->y * deltaTime << std::endl;
            // std::cout << "Position x: " << positions[entity]->x << " Position y: " << positions[entity]->y << std::endl;
            positions[entity]->x += velocities[entity]->x * deltaTime;
            positions[entity]->y += velocities[entity]->y * deltaTime;
        }
    }
    // std::cout << "Movement time: " << clock.getElapsedTime().asMilliseconds() << "ms" << std::endl;
}

void client::collision_system(client::Registry &registry)
{
    sf::Clock clock;
    clock.restart();
    auto &drawables = registry.get_components<components::drawable>();
    auto &owners = registry.get_components<components::owner>();

    for (std::size_t entity = 0; entity < drawables.size(); entity++)
    {
        if (drawables[entity] && owners[entity])
        {
            sf::Sprite &projectile = drawables[entity]->sprite;

            if (owners[entity]->owner == OwnerType::PLAYER)
            {
                check_collision(registry, entity, projectile, EntityType::MOB);
            } else if (owners[entity]->owner == OwnerType::ENEMY)
            {
                check_collision(registry, entity, projectile, EntityType::PLAYER);
            }
        }
    }
    // std::cout << "Collision time: " << clock.getElapsedTime().asMilliseconds() << "ms" << std::endl;
}

void client::life_system(client::Registry &registry, Entity clientEntity)
{
    sf::Clock clock;
    clock.restart();
    auto &health = registry.get_components<components::health>();
    sf::RectangleShape &healthBar = registry.get_health_bar();

    for (std::size_t entity = 0; entity < health.size(); entity++)
    {
        if (health[entity])
        {
            if (health[entity]->life <= 0)
                registry.kill_entity(client::Entity(entity));
            else if (clientEntity == entity)
                healthBar.setSize(sf::Vector2f(health[entity]->life, 20));
        }
    }
    // auto &update = registry.get_components<components::update>();
    // auto &positions = registry.get_components<components::position>();
    // for (std::size_t entity = 0; entity < positions.size(); entity++)
    // {
    //     if (update[entity]){
    //         if (update[entity]->update == true){
    //             std::cout << "TRUE" << std::endl;

    //             update[entity]->update = false;
    //         } else {
    //             std::cout << "FALSE" << std::endl;
    //             registry.kill_entity(client::Entity(entity));
    //         }
    //     }
    // }
    // std::cout << "Life time: " << clock.getElapsedTime().asMilliseconds() << "ms" << std::endl;
}

void client::animation_system(client::Registry &registry, float deltaTime)
{
    sf::Clock clock;
    clock.restart();
    auto &animators = registry.get_components<client::components::AnimatorComponent>();

    for (std::size_t entity = 0; entity < animators.size(); ++entity)
    {
        if (animators[entity])
            animators[entity]->animator.update(deltaTime);
    }

    std::cout << "Animation system time: " << clock.getElapsedTime().asMilliseconds() << "ms" << std::endl;
}

void client::animation_event_system(client::Registry &registry, float deltaTime)
{
    sf::Clock clock;
    clock.restart();
    // std::cout << "Animation event system" << std::endl;
    static std::unordered_map<std::size_t, components::position> previousPositions;
    // std::cout << "Previous positions size: " << previousPositions.size() << std::endl;

    auto &positions = registry.get_components<components::position>();
    // std::cout << "Positions size: " << positions.size() << std::endl;
    auto &animators = registry.get_components<client::components::AnimatorComponent>();
    // std::cout << "Animators size: " << animators.size() << std::endl;
    auto &types = registry.get_components<components::type>();
    // std::cout << "Types size: " << types.size() << std::endl;

    for (std::size_t entity = 0; entity < positions.size(); ++entity)
    {
        // std::cout << "Before checking entity" << std::endl;
        if (!positions[entity] || !animators[entity] || !types[entity] || entity >= positions.size() ||
            entity >= animators.size() || entity >= types.size())
            continue;
        // std::cout << "After checking entity" << std::endl;
        // std::cout << "Entity: " << entity << std::endl;
        auto &currentPosition = *positions[entity];
        // std::cout << "After current position" << std::endl;
        auto &animatorInstance = animators[entity]->animator;
        // std::cout << "After animator instance" << std::endl;
        auto entityType = types[entity]->type;
        // std::cout << "After entity type" << std::endl;

        float velocityX = 0.0f;
        float velocityY = 0.0f;

        if (previousPositions.find(entity) != previousPositions.end())
        {
            // std::cout << "Previous position found" << std::endl;
            auto &previousPosition = previousPositions[entity];
            // std::cout << "Previous position: " << previousPosition.x << ", " << previousPosition.y << std::endl;
            velocityX = (currentPosition.x - previousPosition.x) / deltaTime;
            velocityY = (currentPosition.y - previousPosition.y) / deltaTime;
        }
        // std::cout << "Before setting previous position" << std::endl;
        previousPositions[entity] = currentPosition;
        // std::cout << "After setting previous position" << std::endl;

        switch (entityType)
        {
            case EntityType::PLAYER:
            case EntityType::MOB: {
                // std::cout << "Client or Mob" << std::endl;
                if (std::abs(velocityX) > 0.1f || std::abs(velocityY) > 0.1f)
                {
                    // std::cout << "Moving" << std::endl;
                    if (animatorInstance.hasAnimation("move") && animatorInstance.getCurrentAnimation() != "move")
                    {
                        // std::cout << "Playing move" << std::endl;
                        animatorInstance.play("move");
                    }
                } else
                {
                    // std::cout << "Idle" << std::endl;
                    if (animatorInstance.hasAnimation("idle") && animatorInstance.getCurrentAnimation() != "idle")
                    {
                        // std::cout << "Playing idle" << std::endl;
                        animatorInstance.play("idle");
                    }
                }
                // std::cout << "After client or mob" << std::endl;

                break;
            }
            case EntityType::BOSS: {
                // std::cout << "Boss" << std::endl;
                if (animatorInstance.hasAnimation("idle") && animatorInstance.getCurrentAnimation() != "idle")
                    animatorInstance.play("idle");
                // std::cout << "After boss" << std::endl;
                break;
            }
            case EntityType::BULLET: {
                // std::cout << "Bullet" << std::endl;
                if (animatorInstance.hasAnimation("fly") && animatorInstance.getCurrentAnimation() != "fly")
                    animatorInstance.play("fly");
                // std::cout << "After bullet" << std::endl;
                break;
            }
            case EntityType::ORB: {
                //std::cout << "Orb" << std::endl;
                if (animatorInstance.hasAnimation("fly") && animatorInstance.getCurrentAnimation() != "fly")
                    animatorInstance.play("fly");
                //std::cout << "After orb" << std::endl;
                break;
            }
            default: break;
        }
        // std::cout << "After switch" << std::endl;
    }
    std::cout << "Animation event system time: " << clock.getElapsedTime().asMilliseconds() << "ms" << std::endl;
}
