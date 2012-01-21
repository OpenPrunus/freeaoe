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


#include "Terrain.h"

#include <file/SlpFile.h>
#include "DataManager.h"
#include "ResourceManager.h"
#include <file/SlpFrame.h>

namespace res
{
  
Logger& Terrain::log = Logger::getLogger("freeaoe.resource.Terrain");

Terrain::Terrain(unsigned int Id): Resource(Id, TYPE_TERRAIN)
{

}

Terrain::~Terrain()
{

}

const sf::Image& Terrain::getImage(void )
{
  sf::Image *img = slp_->getFrame()->getImage();
  
  img = new sf::Image(*img);

  return *img;
}

void Terrain::load(void )
{
  if (!isLoaded())
  {
    data_ = DataManager::Inst().getTerrain(getId());
  
    if (slp_ == 0)
      slp_ = ResourceManager::Inst()->getSlp(data_.SLP);
    
    slp_->load();
     
    Resource::load();
  }
}


  
}