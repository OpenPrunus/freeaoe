/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  <copyright holder> <email>

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


#ifndef MAPRENDER_H
#define MAPRENDER_H

#include "IRenderableComponent.h"
#include <mechanics/Map.h>

namespace comp
{

const std::string MAP_RENDER("map_render");

class MapRender : public IRenderableComponent
{

public:
  MapRender();
  virtual ~MapRender();
  
  virtual void update(Time time);
  
  virtual void drawOn(GameRenderer &renderer);
  
  void setMap(MapPtr map);
  
private:
  MapPtr map_; //todo root?
  
  unsigned int x_offset_, y_offset_;    //TODO: ScreenPos?
};

typedef boost::shared_ptr<MapRender> MapRenderPtr;

}

#endif // MAPRENDER_H