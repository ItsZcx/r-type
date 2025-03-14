/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Systems
*/

#ifndef SYSTEMS_HPP_
#define SYSTEMS_HPP_

#include "ecs/Registry.hpp"
#include "ecs/components/animatorComponent.hpp"
#include "ecs/components/health.hpp"
#include "ecs/components/owner.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/sprite.hpp"
#include "ecs/components/type.hpp"
#include "ecs/components/update.hpp"
#include "ecs/components/velocity.hpp"

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

namespace client
{
void render_system(Registry &registry, sf::RenderWindow &window);
void movement_system(client::Registry &registry, float &deltaTime);
void collision_system(client::Registry &registry);
void life_system(client::Registry &registry, Entity clientEntity);
void animation_system(client::Registry &registry, float deltaTime);
void animation_event_system(client::Registry &registry, float deltaTime);

}  // namespace client

#endif /* !SYSTEMS_HPP_ */
