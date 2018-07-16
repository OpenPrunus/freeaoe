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

#include "SfmlRenderTarget.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "GraphicRender.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "mechanics/Map.h"

#define SCALE 1.

SfmlRenderTarget::SfmlRenderTarget(sf::RenderTarget &render_target)
{
    renderTarget_ = &render_target;
}

SfmlRenderTarget::~SfmlRenderTarget()
{
}

Size SfmlRenderTarget::getSize(void) const
{
    return sf::Vector2u(renderTarget_->getSize().x / SCALE, renderTarget_->getSize().y / SCALE);
}

void SfmlRenderTarget::setSize(const Size size) const
{
    renderTarget_->setView(sf::View(sf::FloatRect(0, 0, size.width, size.height)));
    m_camera->setViewportSize(size);
}

void SfmlRenderTarget::draw(const sf::Image &image, ScreenPos pos)
{
    sf::Texture texture;


    texture.loadFromImage(image);

    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setScale(SCALE, SCALE);

    sprite.setPosition(pos);

    renderTarget_->draw(sprite);
}

void SfmlRenderTarget::draw(const sf::Texture &texture, ScreenPos pos)
{
    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setScale(SCALE, SCALE);
    sprite.setPosition(pos);

    renderTarget_->draw(sprite);
}

void SfmlRenderTarget::draw(const sf::Drawable &shape)
{
    renderTarget_->draw(shape);
}

void SfmlRenderTarget::draw(const ScreenRect &rect, const sf::Color &fillColor, const sf::Color &outlineColor, const float outlineSize)
{
    sf::RectangleShape shape;

    shape.setOutlineColor(outlineColor);
    shape.setOutlineThickness(outlineSize);
    shape.setFillColor(fillColor);
    shape.setPosition(rect.topLeft());
    shape.setSize(sf::Vector2f(rect.width, rect.height));

    renderTarget_->draw(shape);
}

void SfmlRenderTarget::display(void)
{
    //  render_window_->Display();
}
