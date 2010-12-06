/****************************************************************************
 * Copyright (C) 2009-2010 GGA Software Services LLC
 * 
 * This file is part of Indigo toolkit.
 * 
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 3 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 ***************************************************************************/

#include "math/algebra.h"
#include "base_cpp/array.h"
#include "base_cpp/obj_array.h"
#include "base_cpp/output.h"
#include "graph/graph_highlighting.h"
#include "base_cpp/reusable_obj_array.h"
#include "layout/metalayout.h"
#include "molecule/molecule.h"
#include "molecule/query_molecule.h"
#include "reaction/reaction.h"
#include "reaction/query_reaction.h"
#include "render_context.h"
#include "render_item.h"
#include "render_item_factory.h"
#include "render.h"

using namespace indigo;

Render::Render (RenderContext& rc, RenderItemFactory& factory) : 
   _rc(rc), _settings(rc.getRenderSettings()), _cnvOpt(rc.cnvOpt), _opt(rc.opt), 
   _factory(factory), minMarg(2)
{}

Render::~Render()
{}

float Render::_getObjScale (int item)
{
   float avgBondLength = 1.0f;
   int bondCount = _factory.getItem(item).getBondCount();
   if (bondCount > 0) {
      avgBondLength = _factory.getItem(item).getTotalBondLength() / bondCount;
   } else {
      int atomCount = _factory.getItem(item).getAtomCount();
      if (atomCount > 0)
         avgBondLength = _factory.getItem(item).getTotalClosestAtomDistance() / atomCount;
   }
   float objScale = 1 / avgBondLength;
   return objScale;
}