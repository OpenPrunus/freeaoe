/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "UnitManager.h"

#include "CompMapObject.h"
#include "CompUnitData.h"
#include "ActionMove.h"
#include "ActionBuild.h"
#include "render/SfmlRenderTarget.h"
#include "resource/LanguageManager.h"
#include "global/Constants.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <iostream>

UnitManager::UnitManager()
{
}

UnitManager::~UnitManager()
{
}

void UnitManager::add(Unit::Ptr unit)
{
    m_units.insert(unit);
//    m_units.insert(unit);
}

bool UnitManager::init()
{
    m_moveTargetMarker = std::make_shared<MoveTargetMarker>();

    return true;
}

bool UnitManager::update(Time time)
{
    bool updated = false;

    for (Unit::Ptr unit : m_units) {
        updated = unit->update(time) || updated;
    }

    updated = m_moveTargetMarker->update(time) || updated;

    return updated;
}

void UnitManager::render(std::shared_ptr<SfmlRenderTarget> renderTarget)
{
    if (m_outlineOverlay.getSize() != renderTarget->getSize()) {
        m_outlineOverlay.create(renderTarget->getSize().x, renderTarget->getSize().y);
    }

    m_outlineOverlay.clear(sf::Color::Transparent);

    for (Unit::Ptr unit : m_units) {
        if (!(unit->data.OcclusionMode & genie::Unit::OccludeOthers)) {
            continue;
        }
        const ScreenPos unitPosition = renderTarget->camera()->absoluteScreenPos(unit->position);

        ScreenRect unitRect = unit->renderer().rect() + unitPosition;

        for (const Unit::Annex &annex : unit->annexes) {
            const ScreenPos annexPosition = renderTarget->camera()->absoluteScreenPos(unit->position + annex.offset);
            ScreenRect annexRect = annex.unit->renderer().rect() + annexPosition;
            if (!renderTarget->camera()->isVisible(annexRect)) {
                continue;
            }
            annex.unit->renderer().render(m_outlineOverlay,
                                          renderTarget->camera()->absoluteScreenPos(unit->position + annex.offset),
                                          comp::RenderType::Base
                                          );
        }
        if (renderTarget->camera()->isVisible(unitRect)) {
            unit->renderer().render(m_outlineOverlay, unitPosition, comp::RenderType::Base);
        }
    }

    for (const Unit::Ptr &unit : m_units) {
        if (!renderTarget->camera()->isVisible(MapRect(unit->position.x, unit->position.y, 10, 10))) {
            continue;
        }

        if (m_selectedUnits.count(unit)) { // draw health indicator
            sf::RectangleShape rect;
            sf::CircleShape circle;
            circle.setFillColor(sf::Color::Transparent);
            circle.setOutlineColor(sf::Color::White);
            circle.setOutlineThickness(1);

            double width = unit->data.OutlineSize.x * Constants::TILE_SIZE_HORIZONTAL;
            double height =  unit->data.OutlineSize.y * Constants::TILE_SIZE_VERTICAL;

            if (unit->data.ObstructionType == genie::Unit::UnitObstruction) {
                width /= 2.;
                height /= 2.;
            } else {
                circle.setPointCount(4);
            }

            ScreenPos pos = renderTarget->camera()->absoluteScreenPos(unit->position);

            circle.setPosition(pos.x - width, pos.y - height);
            circle.setRadius(width);
            circle.setScale(1, height / width);
            renderTarget->draw(circle);

            circle.setPosition(pos.x - width, pos.y - height + 1);
            circle.setOutlineColor(sf::Color::Black);
            renderTarget->draw(circle);

            pos.x -= Constants::TILE_SIZE_HORIZONTAL / 8;
            pos.y -= height + Constants::TILE_SIZE_HEIGHT * unit->data.HPBarHeight;

            rect.setOutlineColor(sf::Color::Transparent);
            rect.setPosition(pos);

            if (unit->creationProgress() < 1.) {
                rect.setFillColor(sf::Color::Red);
                rect.setSize(sf::Vector2f(Constants::TILE_SIZE_HORIZONTAL / 4., 2));
                m_outlineOverlay.draw(rect);
            }

            rect.setFillColor(sf::Color::Green);
            rect.setSize(sf::Vector2f(unit->creationProgress() * Constants::TILE_SIZE_HORIZONTAL / 4., 2));
            m_outlineOverlay.draw(rect);
        }

        if (!(unit->data.OcclusionMode & genie::Unit::ShowOutline)) {
            continue;
        }

        unit->renderer().render(*renderTarget->renderTarget_,
                                renderTarget->camera()->absoluteScreenPos(unit->position),
                                comp::RenderType::Base);

        const ScreenPos pos = renderTarget->camera()->absoluteScreenPos(unit->position);
        unit->renderer().render(m_outlineOverlay, pos, comp::RenderType::Outline);
    }

    m_outlineOverlay.display();
    renderTarget->draw(m_outlineOverlay.getTexture(), ScreenPos(0, 0));

    m_moveTargetMarker->renderer().render(*renderTarget->renderTarget_,
                                          renderTarget->camera()->absoluteScreenPos(m_moveTargetMarker->position),
                                          comp::RenderType::Base);

    if (m_buildingToPlace) {
        m_buildingToPlace->renderer().render(*renderTarget->renderTarget_,
                                             renderTarget->camera()->absoluteScreenPos(m_buildingToPlace->position),
                                             comp::RenderType::ConstructAvailable);
    }
}

void UnitManager::onLeftClick(const MapPos &mapPos)
{
    if (m_buildingToPlace) {
        for (const Unit::Ptr &unit : m_selectedUnits) {
            unit->setCurrentAction(act::MoveOnMap::moveUnitTo(unit, m_buildingToPlace->position, m_map, this));
            unit->queueAction(std::make_shared<act::ActionBuild>(unit, m_buildingToPlace));
        }

        m_units.insert(m_buildingToPlace);
        m_buildingToPlace->setCreationProgress(0);

        m_buildingToPlace.reset();
    }
}

void UnitManager::onRightClick(const MapPos &mapPos)
{
    if (m_selectedUnits.empty()) {
        return;
    }


    for (const Unit::Ptr &unit : m_selectedUnits) {
        unit->setCurrentAction(act::MoveOnMap::moveUnitTo(unit, mapPos, m_map, this));
    }

    m_moveTargetMarker->moveTo(mapPos);
}

void UnitManager::onMouseMove(const MapPos &mapPos)
{
    if (m_buildingToPlace) {
        const int x = std::round(mapPos.x / Constants::TILE_SIZE);
        const int y = std::round(mapPos.y / Constants::TILE_SIZE);
        m_buildingToPlace->position.x = x * Constants::TILE_SIZE;
        m_buildingToPlace->position.y = y * Constants::TILE_SIZE;
    }
}

void UnitManager::selectUnits(const ScreenRect &selectionRect, const CameraPtr &camera)
{
    m_selectedUnits.clear();

    std::vector<Unit::Ptr> containedUnits;
    int8_t requiredInteraction = genie::Unit::ObjectInteraction;
    for (Unit::Ptr unit : m_units) {
        if (!selectionRect.overlaps(unit->rect() + camera->absoluteScreenPos(unit->position))) {
            continue;
        }

        requiredInteraction = std::max(unit->data.InteractionMode, requiredInteraction);
        containedUnits.push_back(unit);
    }

    for (Unit::Ptr unit : containedUnits) {
        if (unit->data.InteractionMode < requiredInteraction) {
            continue;
        }

        std::cout << "Selected " << unit->readableName << " at " << unit->position << " " << unit->renderer().angle() << " " << unit->renderer().graphic_->data_.SLP << std::endl;
        m_selectedUnits.insert(unit);
    }
    if (m_selectedUnits.empty()) {
        std::cout << "Unable to find anything to select in " << selectionRect << std::endl;
    }
}

void UnitManager::setMap(MapPtr map)
{
    m_map = map;
}

void UnitManager::placeBuilding(const Unit::Ptr &unit)
{
    m_buildingToPlace = unit;
}
